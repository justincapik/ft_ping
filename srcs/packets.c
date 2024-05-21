#include "ft_ping.h"

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

c_icmphdr   *create_icmp_packet(char *buffer)
{
    // create and fill icmp package
    c_icmphdr *icmp_hdr = (c_icmphdr *)buffer;
    bzero(buffer, BUFFER_SIZE);
    icmp_hdr->type = ICMP_ECHO;
    icmp_hdr->code = 0;
    icmp_hdr->id = 0xdead;
    icmp_hdr->sequence = 0;
    icmp_hdr->cksum = 0;
    icmp_hdr->cksum = checksum(icmp_hdr, BUFFER_SIZE);

    return icmp_hdr;
}

void        *update_packet(c_icmphdr *icmp_hdr)
{
    icmp_hdr->sequence = icmp_hdr->sequence + 1;
    //icmp_hdr->id = 0xdead;
    icmp_hdr->cksum = 0;
    icmp_hdr->cksum = checksum(icmp_hdr, BUFFER_SIZE);
}