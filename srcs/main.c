#include "ft_ping.h"

int main(int argc, char** argv)
{
    // Check for root access for SOCK_RAW
    if (getuid() != 0)
    {
        fprintf(stderr, "%s: This program requires root privileges!\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    options *opts = parse_argv(argc, argv);
    if (opts == NULL)
    {
        fprintf(stderr, "ft_ping: usage error: Adresse de destination requise\n");
        exit(EXIT_FAILURE);
    }

    char *ip = dns_lookup(argv[1]);
    printf("FT_PING %s (%s)\n", argv[1], ip);

    // create socket destination structure
    struct sockaddr_in endpoint;
    memset(&endpoint, 0, sizeof(endpoint));
    endpoint.sin_family = AF_INET;
    endpoint.sin_addr.s_addr = inet_addr(ip);

    printf("paquet size => %ld\n", sizeof(c_icmphdr));
    printf("addrp size => %ld\n", sizeof(endpoint));
    printf("iphdr size => %ld\n", sizeof(struct iphdr));

    // open socket for icmp paquet
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int ttl = 32;
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

    printf("opened socket\n");
    
    ping_loop(&endpoint, sockfd, opts);
    
    close(sockfd);
    printf("closed socket\n");

    free(opts);

    return (EXIT_SUCCESS);
}