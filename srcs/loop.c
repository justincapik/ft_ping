#include "ft_ping.h"

void    create_sent_info()
{
    struct timeval t;
    gettimeofday( &t, NULL );
    printf("current time s:%ld m:%ld\n", t.tv_sec, t.tv_usec);
}

int     rec_packet(int sockfd, struct sockaddr_in recfrom,
            sentp_info_t *base, options *opts)
{
    struct timeval ct;
    gettimeofday( &ct, NULL );
    socklen_t lenrecfrom = sizeof(recfrom);
    u_int16_t cks;
    char recbuffer[BUFFER_SIZE];
    bzero(recbuffer, BUFFER_SIZE);

    int rres = recvfrom(sockfd, recbuffer, BUFFER_SIZE,
        MSG_PEEK | MSG_DONTWAIT, (struct sockaddr*) &recfrom, &lenrecfrom);
    c_icmphdr *recicmp = (c_icmphdr *)(recbuffer + sizeof(struct iphdr));

    sentp_info_t *old_package; //TODO: parse ICMP error header
    if (rres > 0 && (old_package = check_if_packet_exists(base, recicmp)) != NULL)
    {
        cks = recicmp->cksum;
        recicmp->cksum = 0;
        if (cks != checksum(recicmp, sizeof(*recicmp)))
        {
            printf("ping: error: Invalid checksum\n");
            return (FALSE);
        }
        
        int64_t difftime;
        difftime = (ct.tv_sec - old_package->sent_sec) * 1000000 + ct.tv_usec - old_package->sent_usec;
        old_package->difftime = difftime;

        int rres = recvfrom(sockfd, recbuffer, BUFFER_SIZE,
            MSG_DONTWAIT, (struct sockaddr*) &recfrom, &lenrecfrom);
        struct iphdr *iph = (struct iphdr *)recbuffer;
        c_icmphdr *recicmp = (c_icmphdr *)(recbuffer + sizeof(struct iphdr));
        
        char ip[BUFFER_SIZE];
        inet_ntop(AF_INET, &(iph->saddr), ip, BUFFER_SIZE);
        if (opts->flags & OPTS_NO_HOSTNAME)
            printf("%d bytes from %s: ", rres, ip);
        else 
        { 
            char hostname[256];
            hostname_lookup(recfrom, (char *)hostname); // TODO: catch error
            printf("%d bytes from %s (%s): ", rres, hostname, ip);
        }
        if (opts->flags & OPTS_VERBOSE)
            printf("icmp_seq=%d ident=%d ttl=%d ", recicmp->sequence, recicmp->id, iph->ttl);
        else
            printf("icmp_seq=%d ttl=%d ", recicmp->sequence, iph->ttl);

        printf("time=%lu", difftime/1000);
        if (difftime < 10000)
            printf(".%02lu", (difftime%1000)/10);
        else if (difftime < 100000)
            printf(".%01lu", (difftime%1000)/100);
        printf("ms\n");
        
        return (TRUE);
    }
    return (FALSE);
}

void    sigintHandler(int dummy) {
    keepRunning = FALSE;
    (void)dummy;
}

void    ping_loop(struct sockaddr_in *endpoint, int sockfd, options *opts, packet_stats_t *stats)
{
    struct sockaddr_in recfrom;
    memcpy(&recfrom, endpoint,sizeof(*endpoint));

    struct timeval ct;
    gettimeofday( &ct, NULL );
    
    sentp_info_t *sentp_base = NULL;
    stats->transmitted = 0;
    stats->received = 0;
    stats->start_sec = ct.tv_sec;
    stats->start_usec = ct.tv_usec;
    stats->base = sentp_base;

    char tmp[BUFFER_SIZE];
    char *buffer = (char *)tmp;
    c_icmphdr *icmp_hdr = create_icmp_packet(buffer);

    int count = (opts->count > 0) ? opts->count : 1;
    int ident = rand() % (SHRT_MAX + 1);

    signal(SIGINT, sigintHandler);
    while (count > 0 && keepRunning == TRUE)
    {
        // send packet
        struct timeval t;
        gettimeofday( &t, NULL );
        if (t.tv_sec > ct.tv_sec && (stats->transmitted < opts->count || opts->count < 0))
        {
            update_packet(icmp_hdr, ident);
            if (sendto(sockfd, buffer, BUFFER_SIZE,
                    0, (struct sockaddr*)endpoint, sizeof(*endpoint)) < 0)
            {
                fprintf(stderr, "ping: error: could not send message\n");
                break ;
            }
            gettimeofday( &ct, NULL );
            stats->transmitted++;
            add_p_to_list(&sentp_base, icmp_hdr->id, icmp_hdr->sequence);
        }
        sleep(0.01);
        // rec packet
        if (rec_packet(sockfd, recfrom, sentp_base, opts) == TRUE)
        {
            stats->received++;
            if (opts->count > 0)
                count--;
        }
    }
    stats->base = sentp_base;
}