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


typedef struct options_s {
    uint64_t    count;
    uint        ttl;
    int         verbose;
    char        *host;
} options;

options     *parse_argv(int argc, char **argv);

#endif