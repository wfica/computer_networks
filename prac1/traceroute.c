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

bool create_recipient(const char *target_ip, struct sockaddr_in * recipient)
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

struct icmp_time
{
    struct icmphdr hdr;
    struct timeval tv; // tv.tv_sec  seconds, tv_tv.usec microseconds
};

bool send_package(const int sockfd, const int seq, const int ttl, const struct sockaddr_in *recipient)
{

    // setting ttl
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) == -1)
    {
        printf("Error while setting TTL.\n%s\n", strerror(errno));
        return false;
    }

    struct icmp_time icmp_pck;
    icmp_pck.hdr.type = ICMP_ECHO;
    icmp_pck.hdr.code = 0;
    icmp_pck.hdr.un.echo.id = getpid();
    icmp_pck.hdr.un.echo.sequence = seq;
    icmp_pck.hdr.checksum = 0;
    if (gettimeofday(&icmp_pck.tv, NULL) < 0)
    {
        printf("gettimeofday() error.\n%s\n", strerror(errno));
        return false;
    }
    icmp_pck.hdr.checksum = compute_icmp_checksum((u_int16_t *)&icmp_pck, sizeof(icmp_pck));

    // sending message
    ssize_t bytes_sent = sendto(sockfd, &icmp_pck, sizeof(icmp_pck), 0,
                                (struct sockaddr*)recipient, sizeof(*recipient));

    switch (bytes_sent)
    {
    case sizeof(icmp_pck):
        break;
    case -1:
        printf("Sending message failed.\n%s\n", strerror(errno));
        return false;
    default:
        printf("%lu bytes sent, expected %lu bytes.\n", bytes_sent, sizeof(icmp_pck));
        return false;
    }
    return true;
}

struct three_pck
{
    int n_received;
    struct in_addr sin_addr[3];
    struct timeval tv[3];
};

bool reveive_packages(const int sockfd, int seq, struct three_pck *replies)
{
    // creating fd_set;
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    struct timeval tv;
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
        switch (ready)
        {
        case 1:
            break;
        case 0:
            //printf("Timeout.\n");
            return true;
        case -1:
            printf("Error on select().\n%s\n", strerror(errno));
            return false;
        }

        while (recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
                        (struct sockaddr *)&sender, &sender_len) > 0)
        {
            struct iphdr *ip_header = (struct iphdr *)buffer;
            u_int8_t *icmp_packet = buffer + 4 * ip_header->ihl;
            struct icmphdr *icmp_header = (struct icmphdr *)icmp_packet;
            if (icmp_header->type == ICMP_ECHOREPLY &&
                icmp_header->un.echo.id == getpid() &&
                icmp_header->un.echo.sequence >= seq &&
                icmp_header->un.echo.sequence < seq + 3)
            {
                replies->sin_addr[replies->n_received] = sender.sin_addr;

                replies->n_received++;
                if (replies->n_received == 3)
                    return true;
            }
        }
    }
    return true;
}

void print_replies(const struct three_pck *replies)
{
    for (int i = 0; i < replies->n_received; ++i)
    {
        char sender_ip_str[20];
        inet_ntop(AF_INET, &(replies->sin_addr[i]), sender_ip_str,
                  sizeof(sender_ip_str));
        printf("Received IP packet with ICMP content from: %s\n", sender_ip_str);
    }
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
        for (int i = 0; i < 3; ++i)
            if (send_package(sockfd, seq + i, ttl, &recipient) == false)
                return 1;

        struct three_pck replies;
        replies.n_received = 0;
        if (reveive_packages(sockfd, seq, &replies) == false)
            return 1;

        print_replies(&replies);
        printf("\n\n");
    }
    return 0;
}
