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
    icmp_hdr->id = getpid(); //TODO: make random
    icmp_hdr->sequence = 0;
    icmp_hdr->cksum = 0;
    icmp_hdr->cksum = checksum(icmp_hdr, BUFFER_SIZE);

    return icmp_hdr;
}

void        update_packet(c_icmphdr *icmp_hdr)
{
    icmp_hdr->sequence = icmp_hdr->sequence + 1;
    icmp_hdr->id = 12345;
    icmp_hdr->cksum = 0;
    icmp_hdr->cksum = checksum(icmp_hdr, BUFFER_SIZE);
}

void        add_p_to_list(sentp_info_t **base, u_int16_t id, u_int16_t seq)
{
    sentp_info_t *new = NULL;

    new = (sentp_info_t *)malloc(sizeof(sentp_info_t));
    
    struct timeval ct;
    gettimeofday( &ct, NULL );
    
    new->id = id;
    new->seq = seq;
    new->sent_sec = ct.tv_sec;
    new->sent_usec = ct.tv_usec;
    new->next = NULL;


    if (*base == NULL)
        *base = new;
    else
    {
        sentp_info_t *tmp = *base;
        while (tmp->next != NULL)
            tmp = tmp->next;
        tmp->next = new;
    }
}

// returns sent time
sentp_info_t     *check_if_packet_exists(sentp_info_t **base, c_icmphdr *recicmp)
{
    sentp_info_t    *tmp = *base;
    sentp_info_t    *old = NULL;

    while (tmp != NULL)
    {
        if (tmp->id == recicmp->id && tmp->seq == recicmp->sequence)
        {
            if (old == NULL)
            {
                *base = tmp->next;
                return tmp;
            }
            else
            {
                old->next = tmp->next;
                return tmp;
            }
        }
        old = tmp;
        tmp = tmp->next;
    }
    return NULL;
}

void    print_sentp_info(sentp_info_t *base)
{
    sentp_info_t *tmp = base;
    
    printf("LIST:\n");

    if (tmp == NULL)
        printf("Empty\n");

    int i = 0; 
    while (tmp != NULL && i < 10)
    {
        printf("(%d) id:%d seq:%d sec:%ld, usec:%ld, next:%p, this:%p\n", i,
            tmp->id, tmp->seq, tmp->sent_sec, tmp->sent_usec, tmp->next, tmp);
        tmp = tmp->next;
        ++i;
    }
}