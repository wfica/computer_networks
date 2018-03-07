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
        printf("%u bytes sent, expected %u bytes.\n");
        return 1;
    }


    return 0;
}