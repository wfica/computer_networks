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
#include "receiving.h"

// returns
//          -1 on error
//          0 if target ip has NOT been reached
//          1 if target ip has been reached
int receive_packages(const int sockfd, int seq, struct three_pck *replies)
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

    bool target_reached = false;
    while (true)
    {
        // blocking while waiting for a message
        int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);
        if (gettimeofday(&curr_time, NULL) < 0)
        {
            fprintf(stderr, "gettimeofday() error.\n%s\n", strerror(errno));
            return false;
        }
        switch (ready)
        {
        case 1:
            break;
        case 0:
            //fprintf(stderr, "Timeout.\n");
            return 0;
        case -1:
            fprintf(stderr, "Error on select().\n%s\n", strerror(errno));
            return -1;
        }

        while (recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
                        (struct sockaddr *)&sender, &sender_len) > 0)
        {
            struct iphdr *ip_header = (struct iphdr *)buffer;
            u_int8_t *icmp_packet = buffer + 4 * ip_header->ihl;
            struct icmphdr *icmp_header = (struct icmphdr *)icmp_packet;
            if (icmp_header->type != ICMP_TIME_EXCEEDED && icmp_header->type != ICMP_ECHOREPLY)
                continue;
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
                target_reached = target_reached || (icmp_header->type == ICMP_ECHOREPLY);
                replies->sin_addr[replies->n_received] = sender.sin_addr;
                replies->receiving_time[icmp_header->un.echo.sequence % 3] = curr_time;
                replies->n_received++;
                if (replies->n_received == 3)
                    return target_reached;
            }
        }
    }
    return target_reached;
}