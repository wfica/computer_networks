// Wojciech Fica
// 280180

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/time.h>

#include "utils.h"

u_int16_t compute_icmp_checksum(const void *buff, int length)
{
    u_int32_t sum;
    const u_int16_t *ptr = buff;
    assert(length % 2 == 0);
    for (sum = 0; length > 0; length -= 2)
        sum += *ptr++;
    sum = (sum >> 16) + (sum & 0xffff);
    return (u_int16_t)(~(sum + (sum >> 16)));
}

void print_as_bytes(unsigned char *buff, ssize_t length)
{
    for (ssize_t i = 0; i < length; i++, buff++)
        printf("%.2x ", *buff);
}

bool check_input(int argc, char *argv[])
{
    // checking the number of the arguments
    if (argc != 2)
    {
        printf("Invalid number of arguments.\nUsage: ./traceroute ip_address\n");
        return false;
    }

    // validating IP address
    in_addr_t ip_addr = inet_addr(argv[1]);
    if (ip_addr == (in_addr_t)(-1))
    {
        printf("Invalid IP addres.\nUsage: ./traceroute ip_address\n");
        return false;
    }
    return true;
}

void print_time(const struct timeval *tv)
{
    printf(" %ld s  %ld us\n", tv->tv_sec, tv->tv_usec);
}
// returns difference in time in ms [1 ms = 0.01 s]
long int timeval_substract(const struct timeval *x, const struct timeval *y)
{
    struct timeval res;
    timersub(y, x, &res);
    return res.tv_usec / 1000;
}

void print_replies(const struct three_pck *replies, const int ttl)
{
    printf("%d. ", ttl);
    char sender_ip_str[3][20];
    if (replies->n_received == 0)
    {
        printf("*\n");
        return;
    }
    for (int i = 0; i < 3; ++i)
        assert(inet_ntop(AF_INET, &(replies->sin_addr[i]), sender_ip_str[i], sizeof(sender_ip_str[i])));

    printf("%s ", sender_ip_str[0]);
    if (strcmp(sender_ip_str[0], sender_ip_str[1]) != 0)
        printf("%s ", sender_ip_str[1]);

    if (strcmp(sender_ip_str[0], sender_ip_str[2]) != 0 &&
        strcmp(sender_ip_str[1], sender_ip_str[2]) != 0)
        printf("%s ", sender_ip_str[2]);

    if (replies->n_received != 3)
    {
        printf("???\n");
        return;
    }
    long t = 0;
    for (int i = 0; i < replies->n_received; ++i)
    {
        long t_diff = timeval_substract(&replies->sending_time[i], &replies->receiving_time[i]);
        t += t_diff;
    }
    printf(" %ld ms\n", t / 3L);
}
