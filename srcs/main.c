#include "ft_ping.h"

#define BUFFER_SIZE 2048

int main(int argc, char** argv)
{
    if (argc == 1){
        fprintf(stderr, "%s: usage error: Adresse de destination requise\n", argv[0]);
        exit(EXIT_FAILURE);
    }

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

    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

    char *buffer = (char*)malloc(BUFFER_SIZE);
    struct icmp_header *icmp = (struct icmp_header *)buffer;
    icmp->type = ICMP_ECHO;
    icmp->code = 0;
    icmp->checksum = 0; 
    icmp->id = getpid();
    icmp->sequence = 1;

    struct sockaddr_in endpoint;
    memset(&endpoint, 0, sizeof(endpoint));
    endpoint.sin_family = addr->sin_family;
    endpoint.sin_addr = addr->sin_addr;

    printf("%p\n", buffer);
    sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&endpoint, sizeof(endpoint));
    printf("%p\n", buffer);
    recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*) &endpoint, (socklen_t*)sizeof(endpoint)); 

    printf("%p\n", buffer);

    close(sockfd);
    printf("%p\n", buffer);
}