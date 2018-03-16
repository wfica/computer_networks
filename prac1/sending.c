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
#include "sending.h"

// returns true on successfully creating a recipient
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
        fprintf(stderr, "inet_pton() input is not a vaild IP dotted string.\n");
        return false;
    case -1:
        fprintf(stderr, "inet_pton() error.\n%s\n", strerror(errno));
        return false;
    }
    return true;
}

// sends package and, on success, updates replies (adds time of sending)
// returns whether a package was sent successfully
bool send_package(const int sockfd, const int seq, const int ttl,
                  const struct sockaddr_in *recipient, struct three_pck *replies)
{

    // setting ttl
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) == -1)
    {
        fprintf(stderr, "Error while setting TTL.\n%s\n", strerror(errno));
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
        fprintf(stderr, "gettimeofday() error.\n%s\n", strerror(errno));
        return false;
    }
    switch (bytes_sent)
    {
    case sizeof(icmp_hdr):
        break;
    case -1:
        fprintf(stderr, "Sending message failed.\n%s\n", strerror(errno));
        return false;
    default:
        fprintf(stderr, "%lu bytes sent, expected %lu bytes.\n", bytes_sent, sizeof(icmp_hdr));
        return false;
    }
    return true;
}