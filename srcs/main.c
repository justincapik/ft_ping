#include "ft_ping.h"

int main(int argc, char** argv)
{
    // Check for root access for SOCK_RAW
    if (getuid() != 0)
    {
        fprintf(stderr, "%s: This program requires root privileges!\n", argv[0]);
        return (EXIT_FAILURE);
    }

    options opts;
    if (parse_argv(argc, argv, &opts) == FALSE)
        return (EXIT_FAILURE);

    // open socket for icmp paquet
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int ttl = opts.ttl;
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));
    if (opts.flags & OPTS_VERBOSE)
        printf("ping: sock4.fd: %d (socktype: SOCK_RAW)", sockfd);

    char *ip = dns_lookup(argv[argc - 1], &opts);
    printf("FT_PING %s (%s) 56(%d) bytes of data.\n",
        argv[argc - 1], ip, 56 + sizeof(c_icmphdr) + sizeof(struct iphdr));
    if (strcmp(argv[argc - 1], ip) == 0)
        opts.flags |= OPTS_NO_HOSTNAME;

    // create socket destination structure
    struct sockaddr_in endpoint;
    memset(&endpoint, 0, sizeof(endpoint));
    endpoint.sin_family = AF_INET;
    endpoint.sin_addr.s_addr = inet_addr(ip);
    
    packet_stats_t stats;
    ping_loop(&endpoint, sockfd, &opts, &stats);

    printf("\n--- %s ping statistics ---\n", argv[argc - 1]);
    printf("%d packets transmitted, %d received, %d%% packet loss\n",
        stats.transmitted, stats.received,
        (int)(100.0 * ((float)(stats.transmitted - stats.received) / (float)stats.transmitted)));

    close(sockfd);

    return (EXIT_SUCCESS);
}