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
    if (opts == NULL)
    {
        fprintf(stderr, "ft_ping: usage error: Adresse de destination requise\n");
        exit(EXIT_FAILURE);
    }

    // DNS lookup
    struct addrinfo hint;
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_INET;    /* Allow IPv4 or IPv6 */
    hint.ai_socktype = SOCK_RAW; /* Datagram socket */
    hint.ai_flags = AI_CANONNAME;
    hint.ai_protocol = IPPROTO_ICMP;          /* Any protocol */
    hint.ai_canonname = NULL;
    hint.ai_addr = NULL;
    hint.ai_next = NULL;
    struct addrinfo add_res;
    struct addrinfo* res = &add_res;
    int s = getaddrinfo(argv[1], 0, &hint, &res);
    if (s != 0) {
        fprintf(stderr, "ft_ping: %s: %s\n", argv[1], gai_strerror(s));
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in *addr;
    addr = (struct sockaddr_in *)res->ai_addr; 
    char *ip = inet_ntoa((struct in_addr)addr->sin_addr);

    printf("FT_PING %s (%s)\n", argv[1], ip);

    // create and fill icmp package
    char buffer[BUFFER_SIZE];
    struct icmp *icmp_hdr = (struct icmp *)buffer;
    bzero(buffer, BUFFER_SIZE);
    icmp_hdr->icmp_type = ICMP_ECHO;
    icmp_hdr->icmp_code = 0;
    icmp_hdr->icmp_id = 0xadde;
    icmp_hdr->icmp_seq = 1;
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

    // open socket for icmp paquet
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    int ttl = 32;
    setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));

    printf("opened socket\n");
    
    char recbuffer[BUFFER_SIZE];
    bzero(recbuffer, BUFFER_SIZE);

    for(int i = (opts->count < 0) ? 100000 : opts->count; i < 100; ++i)
    {
        icmp_hdr->icmp_seq = i+1;
        icmp_hdr->icmp_cksum = 0;
        icmp_hdr->icmp_cksum = checksum(icmp_hdr, BUFFER_SIZE);

        // send paquet
        int sres = sendto(sockfd, buffer, BUFFER_SIZE,
            0, (struct sockaddr*)&endpoint, sizeof(endpoint));
        printf("sentto  (%d)\n", sres);
        
        // receive paquet
        // pick() and check id number
        int rres = recvfrom(sockfd, recbuffer, BUFFER_SIZE,
            MSG_DONTWAIT, (struct sockaddr*) &recfrom, &lenrecfrom); 
        printf("recvfrom(%d)\n", rres);

        //getnameinfo(&sa, sizeof(sa), hostname, size_hostname, NULL, 0, 0);

        // check checksum
        // signal(SIGINT, func_to_catch_ctrl_c);
        // 

        sleep(1);

        // get hostname and dns from addr
        char revhostname[1000];
        bzero(revhostname, 1000);
        int ret = getnameinfo((struct sockaddr*)&endpoint, (socklen_t)sizeof(struct sockaddr),
                        revhostname, 1000, 0, 0, NI_NOFQDN);
        printf("hostname: [%s]\n", revhostname);
    }    
    close(sockfd);
    printf("closed socket\n");

    free(opts);

    return (EXIT_SUCCESS);
}