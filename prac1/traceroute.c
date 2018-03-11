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

void print_time(const struct timeval *tv){
    printf(" %ld s  %ld us\n", tv->tv_sec, tv->tv_usec);
}

bool create_recipient(const char *target_ip, struct sockaddr_in *recipient)
{
    bzero(recipient, sizeof(*recipient));
    recipient->sin_family = AF_INET;

    // converting an address in its standard text representation into its numeric binary form
    switch (inet_pton(AF_INET, target_ip, &(recipient->sin_addr)))
    {
    case 1:
        break;
    case 0:
        printf("inet_pton() input is not a vaild IP dotted string.\n");
        return false;
    case -1:
        printf("inet_pton() error.\n%s\n", strerror(errno));
        return false;
    }
    return true;
}

struct three_pck
{
    int n_received;
    struct in_addr sin_addr[3];
    struct timeval sending_time[3];
    struct timeval receiving_time[3];
};

// returns difference in time in ms [ms = 0.01 s]
long int timeval_substract(const struct timeval *x, const struct timeval *y)
{
    //printf("%ld\n%ld\n", x->tv_sec, y->tv_sec);
    struct timeval res;
    timersub(x, y, &res);
    return res.tv_usec / 1000;
}

bool send_package(const int sockfd, const int seq, const int ttl,
                  const struct sockaddr_in *recipient, struct three_pck *replies)
{

    // setting ttl
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) == -1)
    {
        printf("Error while setting TTL.\n%s\n", strerror(errno));
        return false;
    }
    //creating icmp header
    struct icmphdr icmp_hdr;
    icmp_hdr.type = ICMP_ECHO;
    icmp_hdr.code = 0;
    icmp_hdr.un.echo.id = getpid();
    icmp_hdr.un.echo.sequence = seq;
    icmp_hdr.checksum = 0;
    icmp_hdr.checksum = compute_icmp_checksum((u_int16_t *)&icmp_hdr, sizeof(icmp_hdr));

    // sending message
    ssize_t bytes_sent = sendto(sockfd, &icmp_hdr, sizeof(icmp_hdr), 0,
                                (struct sockaddr *)recipient, sizeof(*recipient));

    if (gettimeofday(&replies->sending_time[seq % 3], NULL) < 0)
    {
        printf("gettimeofday() error.\n%s\n", strerror(errno));
        return false;
    }
    print_time(&replies->sending_time[seq % 3]);
    switch (bytes_sent)
    {
    case sizeof(icmp_hdr):
        break;
    case -1:
        printf("Sending message failed.\n%s\n", strerror(errno));
        return false;
    default:
        printf("%lu bytes sent, expected %lu bytes.\n", bytes_sent, sizeof(icmp_hdr));
        return false;
    }
    return true;
}

/*
 returns 
    1 if target has been reached
    -1 if an error occured
    0 otherwise
*/
int reveive_packages(const int sockfd, int seq, struct three_pck *replies)
{
    // creating fd_set;
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    struct timeval tv, curr_time;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    // creating sender
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    u_int8_t buffer[IP_MAXPACKET];

    while (true)
    {
        // blocking while waiting for a message
        int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
        if (gettimeofday(&curr_time, NULL) < 0)
        {
            printf("gettimeofday() error.\n%s\n", strerror(errno));
            return false;
        }
        print_time(&curr_time);
        switch (ready)
        {
        case 1:
            break;
        case 0:
            //printf("Timeout.\n");
            return 0;
        case -1:
            printf("Error on select().\n%s\n", strerror(errno));
            return -1;
        }

        bool target_reached = false;
        while (recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
                        (struct sockaddr *)&sender, &sender_len) > 0)
        {
            struct iphdr *ip_header = (struct iphdr *)buffer;
            u_int8_t *icmp_packet = buffer + 4 * ip_header->ihl;
            struct icmphdr *icmp_header = (struct icmphdr *)icmp_packet;
            if (icmp_header->type != ICMP_TIME_EXCEEDED && icmp_header->type != ICMP_ECHOREPLY)
                continue;
            printf("!!!\n");
            target_reached = target_reached || (icmp_header->type == ICMP_ECHOREPLY);
            if (icmp_header->type == ICMP_TIME_EXCEEDED)
            {
                u_int8_t *ptr = (u_int8_t *)icmp_header;
                ptr += 8; // move by 8 bytes
                ip_header = (struct iphdr *)ptr;
                icmp_packet = ptr + 4 * ip_header->ihl;
                icmp_header = (struct icmphdr *)icmp_packet;
            }
            if (icmp_header->un.echo.id == getpid() &&
                icmp_header->un.echo.sequence >= seq &&
                icmp_header->un.echo.sequence < seq + 3)
            {
                replies->sin_addr[replies->n_received] = sender.sin_addr;
                replies->receiving_time[icmp_header->un.echo.sequence % 3] = curr_time;
                replies->n_received++;
                if (replies->n_received == 3)
                    return target_reached;
            }
        }
    }
    return 0;
}

void print_replies(const struct three_pck *replies, const int ttl)
{
    printf("%d. ", ttl);
    char sender_ip_str[3][20];
    if (replies->n_received == 0)
    {
        printf("\n");
        return;
    }
    for (int i = 0; i < 3; ++i)
        inet_ntop(AF_INET, &(replies->sin_addr[i]), sender_ip_str[i], sizeof(sender_ip_str[i]));

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
        printf(" -- %ld\n", t_diff);
    }
    printf(" %ld ms\n", t / 3L);
}

int main(int argc, char *argv[])
{
    if (!check_input(argc, argv))
        return 1;

    // opening raw socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd == -1)
    {
        printf("Error while opening socket.\n%s\n", strerror(errno));
        return 1;
    }

    // creating recipient
    struct sockaddr_in recipient;
    if (create_recipient(argv[1], &recipient) == false)
        return 1;

    for (int seq = 0, ttl = 1; ttl <= 30; ++ttl, seq += 3)
    {
        struct three_pck replies;
        replies.n_received = 0;

        for (int i = 0; i < 3; ++i)
            if (send_package(sockfd, seq + i, ttl, &recipient, &replies) == false)
                return 1;

        int rp = reveive_packages(sockfd, seq, &replies);
        if (rp < 0)
            return 1;

        print_replies(&replies, ttl);
        if (rp == 1)
            break;
    }
    return 0;
}
