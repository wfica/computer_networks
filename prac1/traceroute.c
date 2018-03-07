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

#include "utils.h"


int main(int argc, char *argv[])
{
    // checking the number of the arguments
    if (argc != 2)
    {
        printf("Invalid number of arguments.\nUsage: ./traceroute ip_address\n");
        return 1;
    }

    // validating IP address
    in_addr_t ip_addr = inet_addr(argv[1]);
    if (ip_addr == (in_addr_t)(-1))
    {
        printf("Invalid IP addres.\nUsage: ./traceroute ip_address\n");
        return 1;
    }
    printf("IP: %u\n", ip_addr);

    // opening raw socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd == -1)
    {
        printf("Error while opening socket.\n%s\n", strerror(errno));
        return 1;
    }

    int __seq = 1;

    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = getpid();
    icmp_header.un.echo.sequence = __seq++;
    icmp_header.checksum = 0;
    icmp_header.checksum = compute_icmp_checksum((u_int16_t *)&icmp_header, sizeof(icmp_header));

    // creating recipient
    struct sockaddr_in recipient;
    bzero(&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;

    // converting an address in its standard text representation into its numeric binary form
    switch (inet_pton(AF_INET, argv[1], &recipient.sin_addr))
    {
    case 1:
        break;
    case 0:
        printf("inet_pton() input is not a vaild IP dotted string.\n");
        return 1;
    case -1:
        printf("inet_pton() error.\n%s\n", strerror(errno));
        return 1;
    }

    // setting ttl
    int ttl = 42;
    if (setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) == -1)
    {
        printf("Error while setting TTL.\n%s\n", strerror(errno));
        return 1;
    }

    // sending message
    ssize_t bytes_sent = sendto(sockfd, &icmp_header, sizeof(icmp_header), 0,
                                (struct sockaddr *)&recipient, sizeof(recipient));

    switch (bytes_sent)
    {
    case sizeof(icmp_header):
        break;
    case -1:
        printf("Sending message failed.\n%s\n", strerror(errno));
        return 1;
    default:
        printf("%lu bytes sent, expected %lu bytes.\n", bytes_sent, sizeof(icmp_header));
        return 1;
    }

    //---------------------------------------

    // creating fd_set;
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;

    // blocking while waiting for a message
    int ready = select(sockfd + 1, &descriptors, NULL, NULL, &tv);

    switch (ready)
    {
    case 1:
        break;
    case 0:
        printf("Timeout.\n");
        return -1;
    case -1:
        printf("Error on select().\n%s\n", strerror(errno));
        return -1;
    }

    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    u_int8_t buffer[IP_MAXPACKET];

    // reading messages
    ssize_t packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
                                  (struct sockaddr *)&sender, &sender_len);

    // at least one message must be ready
    if (packet_len <= 0)
    {
        printf("Could not read any message.\n");
        return 1;
    }
    while (packet_len > 0)
    {
        char sender_ip_str[20];
        const char *ip_ptr = inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
        if (ip_ptr == NULL)
        {
            printf("inet_ntop() error.\n%s\n", strerror(errno));
            return 1;
        }
        printf("Received IP packet with ICMP content from: %s\n", sender_ip_str);
        struct iphdr *ip_header = (struct iphdr *)buffer;
        ssize_t ip_header_len = 4 * ip_header->ihl;

        printf("IP header: ");
        print_as_bytes(buffer, ip_header_len);
        printf("\n");

        printf("IP data:   ");
        print_as_bytes(buffer + ip_header_len, packet_len - ip_header_len);
        printf("\n\n");

        // read remaining messages
        packet_len = recvfrom(sockfd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
                              (struct sockaddr *)&sender, &sender_len);
    }

    return 0;
}