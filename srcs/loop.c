#include "ft_ping.h"

void    create_sent_info(sentp_info_t *base, int id, int seq)
{
    struct timeval t;
    gettimeofday( &t, NULL );
    printf("current time s:%d m:%d\n", t.tv_sec, t.tv_usec);
}

int    rec_packet(int sockfd, c_icmphdr *icmp_hdr,
    struct sockaddr_in recfrom, sentp_info_t *base, options *opts)
{
    socklen_t lenrecfrom = sizeof(recfrom);
    u_int16_t cks;
    
    char recbuffer[BUFFER_SIZE];
    bzero(recbuffer, BUFFER_SIZE);

    int rres = recvfrom(sockfd, recbuffer, BUFFER_SIZE,
        MSG_PEEK | MSG_DONTWAIT, (struct sockaddr*) &recfrom, &lenrecfrom);
    struct iphdr *iph = (struct iphdr *)recbuffer;
    c_icmphdr *recicmp = (c_icmphdr *)(recbuffer + sizeof(struct iphdr));

    cks = recicmp->cksum;
    recicmp->cksum = 0;

    if (rres > 0 && recicmp->id == icmp_hdr->id
        && recicmp->sequence == icmp_hdr->sequence)
    {
        if (cks != checksum(recicmp, sizeof(*recicmp)))
            printf("invalid checksum\n"); //TODO: change message

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
            printf("icmp_seq=%d ident=%d ttl=%d\n", recicmp->sequence, recicmp->id, iph->ttl);
        else
            printf("icmp_seq=%d ttl=%d\n", recicmp->sequence, iph->ttl);
        return (TRUE);
    }
    return (FALSE);
}

void    sigintHandler(int dummy) {
    keepRunning = FALSE;
}

void    ping_loop(struct sockaddr_in *endpoint, int sockfd, options *opts, packet_stats_t *stats)
{
    struct sockaddr_in recfrom;
    memcpy(&recfrom, endpoint,sizeof(*endpoint));

    sentp_info_t *sentp_base = NULL;
    stats->transmitted = 0;
    stats->received = 0;
    stats->start_time = 0; //TODO:
    stats->base = NULL;

    char tmp[BUFFER_SIZE];
    char *buffer = (char *)tmp;
    c_icmphdr *icmp_hdr = create_icmp_packet(buffer);

    struct timeval ct;
    gettimeofday( &ct, NULL );
    int count = (opts->count > 0) ? opts->count : 1;
    
    signal(SIGINT, sigintHandler);
    while (count > 0 && keepRunning == TRUE)
    {
        // send packet
        struct timeval t;
        gettimeofday( &t, NULL );
        if (t.tv_sec - ct.tv_sec > 0 && stats->transmitted < opts->count)
        {
            update_packet(icmp_hdr);
            int sres = sendto(sockfd, buffer, BUFFER_SIZE,
                0, (struct sockaddr*)endpoint, sizeof(*endpoint));
            gettimeofday( &ct, NULL );
            stats->transmitted++;
        }
        sleep(0.01);
        // rec packet
        if (rec_packet(sockfd, icmp_hdr, recfrom, sentp_base, opts) == TRUE)
        {
            stats->received++;
            if (opts->count > 0)
                count--;
        }
    }
}