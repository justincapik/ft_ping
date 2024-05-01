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
#include <netinet/ip_icmp.h>  
#include <errno.h> // for errno

struct icmp_header {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t id;
    uint16_t sequence;
};


#endif