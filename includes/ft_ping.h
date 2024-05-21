#ifndef FT_PING_H
# define FT_PING_H

# include <stdio.h>
#include <stdlib.h> //for exit()
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <netdb.h> // for NI_MAXHOST, getnameinfo() and gai_strerror()
#include <netinet/in.h> 
#include <netinet/ip_icmp.h>
#include <errno.h> // for errno
#include <time.h>
#include <sys/time.h>

# define TRUE 0
# define FALSE 1

# define OPTS_VERBOSE 0x1
# define OPTS_COUNT 0x2

# define BUFFER_SIZE 64

typedef struct options_s {
    int64_t     count;
    char        ttl;
    uint64_t    flags;
    char        *host;
    char        *ip;
} options;

typedef struct sent_paquet_info_s {
    u_int16_t                   id;
    u_int16_t                   seq;
    u_int16_t                   cksum;
    int                         sendtime;
    struct send_paquet_info_s   *next;
} sentp_info;

typedef struct custom_icmphdr_s
{
    u_int8_t    type;                /* message type */
    u_int8_t    code;                /* type sub-code */
    u_int16_t   cksum;
    u_int16_t   id;
    u_int16_t   sequence;
    //char        []data; TODO: ?
} c_icmphdr;

options             *parse_argv(int argc, char **argv);
char                *dns_lookup(char *canonname);
char                *hostname_lookup(struct sockaddr_in endpoint);
c_icmphdr           *create_icmp_packet(char *buffer);
void                *update_packet(c_icmphdr *icmp_hdr);
void                ping_loop(struct sockaddr_in *endpoint, int sockfd, options *opts);

#endif