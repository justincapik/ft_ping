#include "ft_ping.h"

void    create_sent_info(sentp_info *base, int id, int seq)
{
    struct timeval t;
    gettimeofday( &t, NULL );
    printf("current time s:%d m:%d\n", t.tv_sec, t.tv_usec);

}

void    ping_loop(struct sockaddr_in *endpoint, int sockfd, options *opts)
{
    struct sockaddr_in recfrom;
    memcpy(&recfrom, endpoint,sizeof(*endpoint));
    socklen_t lenrecfrom = sizeof(recfrom);

    sentp_info *sentp_base = NULL;

    char *buffer = (char*)malloc(sizeof(char) * BUFFER_SIZE);
    c_icmphdr *icmp_hdr = create_icmp_packet(buffer);
    
    char recbuffer[BUFFER_SIZE];
    bzero(recbuffer, BUFFER_SIZE);

    struct timeval ct;
    gettimeofday( &ct, NULL );
    int count = 5;
    while (count > 0)
    {
        // send paquet
        struct timeval t;
        gettimeofday( &t, NULL );
        if (t.tv_sec - ct.tv_sec > 0)
        {
            update_packet(icmp_hdr);

            int sres = sendto(sockfd, buffer, BUFFER_SIZE,
                0, (struct sockaddr*)endpoint, sizeof(*endpoint));
            printf("sendto  (%d) [id: %x, seq: %d]\n",
                sres, icmp_hdr->id, icmp_hdr->sequence);
            gettimeofday( &ct, NULL );
        }
        sleep(0.01);
        
        // receive paquet
        // pick() and check id number
        int rres = recvfrom(sockfd, recbuffer, BUFFER_SIZE,
            MSG_DONTWAIT , (struct sockaddr*) &recfrom, &lenrecfrom);
        c_icmphdr *recicmp = (c_icmphdr *)(recbuffer + sizeof(struct iphdr));
        if (rres > 0 && recicmp->id == icmp_hdr->id
            && recicmp->sequence == icmp_hdr->sequence)
        {
            printf("recvfrom(%d) [id: %x, seq: %d]\n",
                rres, recicmp->id, recicmp->sequence);
            count--;
        }
        // check checksum
        // signal(SIGINT, func_to_catch_ctrl_c);
        // 

        char *hostname = hostname_lookup(*endpoint);
        //printf("hostname: [%s]\n", hostname);
        free(hostname);
    }

    free(buffer); 
}