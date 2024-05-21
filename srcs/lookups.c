#include "ft_ping.h"

char    *dns_lookup(char *canonname)
{
    // check direction of lookup ?
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
    int s = getaddrinfo(canonname, 0, &hint, &res);
    if (s != 0) {
        fprintf(stderr, "ft_ping: %s: %s\n", canonname, gai_strerror(s));
        return NULL;
    }
    struct sockaddr_in *addr;
    addr = (struct sockaddr_in *)res->ai_addr; 
    char *ip = inet_ntoa((struct in_addr)addr->sin_addr);

    return (ip);
}

// get hostname and dns from addr
char    *hostname_lookup(struct sockaddr_in endpoint)
{
        char *revhostname = (char*)malloc(sizeof(char)*1000);
        bzero(revhostname, 1000);
        int ret = getnameinfo((struct sockaddr*)&endpoint, (socklen_t)sizeof(struct sockaddr),
                        revhostname, 1000, 0, 0, NI_NOFQDN);

        return (char*)revhostname;
}