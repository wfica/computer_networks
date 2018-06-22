// Wojciech Fica
// 280180

#ifndef UTILS_H
#define UTILS_H

#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <sys/select.h>

#define MX_ALLOWED_LEN 1000
#define MX_QUERIES_SEND_AT_ONCE 200

struct file_part
{
    size_t len;
    char buffer[MX_ALLOWED_LEN + 16];
    file_part(char *data_start, size_t length);
};

int init(int port);
void send_queries(const int fd, const sockaddr_in server_addr, int bytes_received, int total_len);

#endif
