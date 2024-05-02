#include "ft_ping.h"

#define BUFFER_SIZE 64

unsigned short checksum(void *b, int len) {
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;

    return result;
}

int main(int argc, char** argv)
{
    // Check for root access
    if (getuid() != 0)
    {
        fprintf(stderr, "%s: This program requires root privileges!\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    options *opts = parse_argv(argc, argv);
    //if (opts == NULL)
    //    exit(EXIT_FAILURE);

    // DNS lookup
    struct addrinfo hint;
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hint.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hint.ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hint.ai_protocol = 0;          /* Any protocol */
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    struct addrinfo* res = malloc(sizeof(struct addrinfo));
    int s = getaddrinfo(argv[1], NULL, &hint, &res);
    if (s != 0) {
        fprintf(stderr, "ft_ping: %s: %s\n", argv[1], gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in *addr;
    addr = (struct sockaddr_in *)res->ai_addr; 
    char *ip = inet_ntoa((struct in_addr)addr->sin_addr);

    printf("FT_PING %s (%s)\n", argv[1], ip);

    // create and fill icmp package
    char *buffer = (char*)malloc(BUFFER_SIZE);
    struct icmp *icmp_hdr = (struct icmp *)buffer;
    icmp_hdr->icmp_type = ICMP_ECHO;
    icmp_hdr->icmp_code = 0;
    icmp_hdr->icmp_id = htons(getpid());
    icmp_hdr->icmp_seq = htons(1);
    icmp_hdr->icmp_cksum = 0;
    icmp_hdr->icmp_cksum = checksum(icmp_hdr, BUFFER_SIZE);

    // create socket destination structure
    struct sockaddr_in endpoint;
    memset(&endpoint, 0, sizeof(endpoint));
    endpoint.sin_family = AF_INET;
    endpoint.sin_addr.s_addr = inet_addr(ip);
    struct sockaddr_in recfrom;
    memcpy(&recfrom, &endpoint,sizeof(endpoint));
    socklen_t lenrecfrom = sizeof(recfrom);

    printf("paquet size => %ld\n", sizeof(*icmp_hdr));
    printf("addrp size => %ld\n", sizeof(endpoint));

    for (int i = 0; i < 5; ++i)
    {

        // open socket for icmp paquet
        int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
        printf("opened socket\n");
        
        // send paquet
        int sres = sendto(sockfd, buffer, BUFFER_SIZE,
            0, (struct sockaddr*)&endpoint, sizeof(endpoint));
        printf("sentto  (%d)\n", sres);
        
        // receive paquet
        int rres = recvfrom(sockfd, buffer, BUFFER_SIZE,
            0, (struct sockaddr*) &recfrom, &lenrecfrom); 
        printf("recvfrom(%d)\n", rres);
        
        close(sockfd);
        printf("closed socket\n");
    }


    return (EXIT_SUCCESS);
}