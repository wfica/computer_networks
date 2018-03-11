// Wojciech Fica
// 280180

#ifndef UTILS_H
#define UTILS_H

#include <arpa/inet.h>
#include <stdbool.h>

struct three_pck
{
    int n_received;
    struct in_addr sin_addr[3];
    struct timeval sending_time[3];
    struct timeval receiving_time[3];
};

u_int16_t compute_icmp_checksum(const void *buff, int length);
void print_as_bytes(unsigned char *buff, ssize_t length);
bool check_input(int argc, char *argv[]);
void print_time(const struct timeval *tv);
void  print_replies(const struct three_pck *replies, const int ttl);
long int timeval_substract(const struct timeval *x, const struct timeval *y);

#endif