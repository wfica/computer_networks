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
#include "receiving.h"

int main(int argc, char *argv[])
{
    if (!check_input(argc, argv))
        return 1;

    // opening raw socket
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd == -1)
    {
        fprintf(stderr, "Error while opening socket.\n%s\n", strerror(errno));
        return 1;
    }

    // creating recipient
    struct sockaddr_in recipient;
    if (create_recipient(argv[1], &recipient) == false)
        return 1;

    const int loops = 30;
    for (int seq = 0, ttl = 1; ttl <= loops; ++ttl, seq += 3)
    {
        struct three_pck replies;
        replies.n_received = 0;

        for (int i = 0; i < 3; ++i)
            if (send_package(sockfd, seq + i, ttl, &recipient, &replies) == false)
                return 1;

        int rp = receive_packages(sockfd, seq, &replies);
        if (rp < 0)
            return 1;

        print_replies(&replies, ttl);
        if (rp == 1)
            break;
    }
    return 0;
}
