#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define MX_ALLOWED_LEN 1000
#define WINDOW_SIZE 1024
#define MX_QUERIES_SEND_AT_ONCE 500
#define DEBUG 0

#define debug(msg)               \
    do                           \
    {                            \
        if (DEBUG)               \
        {                        \
            cout << msg << endl; \
        }                        \
    } while (0)

using namespace std;

int init(int port)
{

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return -1;
    }

    sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        fprintf(stderr, "bind error: %s\n", strerror(errno));
        return -1;
    }
    return fd;
}

void send_queries(const int fd, const sockaddr_in server_addr, int bytes_received, int total_len)
{
    int bytes_rem = total_len - bytes_received;
    int num_queries = min(MX_QUERIES_SEND_AT_ONCE, (bytes_rem + MX_ALLOWED_LEN - 1) / MX_ALLOWED_LEN);
    for (int i = 0; i < num_queries; ++i)
    {
        int start = bytes_received + i * MX_ALLOWED_LEN;
        int len = min(total_len - start, MX_ALLOWED_LEN);
        string msg = "GET " + to_string(start) + " " + to_string(len) + "\n";
        sendto(fd, msg.c_str(), msg.length(), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    }
}

struct file_part
{
    size_t len;
    char buffer[MX_ALLOWED_LEN + 24];
    file_part(char *data_start, size_t length)
    {
        len = length;
        memcpy(buffer, data_start, length);
    }
};

unordered_map<size_t, file_part> window;

size_t save_data(FILE *pFile, size_t bytes_received)
{
    unordered_map<size_t, file_part>::iterator it = window.find(bytes_received);
    while (it != window.end())
    {
        fwrite(it->second.buffer, sizeof(char), it->second.len, pFile);
        bytes_received += it->second.len;
        window.erase(it);
        it = window.find(bytes_received);
    }
    return bytes_received;
}

size_t collect_responses(const int fd, const sockaddr_in server_addr, size_t bytes_received, FILE *pFile)
{
    char buffer[IP_MAXPACKET];
    sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);
    while (recvfrom(fd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr *)&sender, &sender_len) > 0)
    {
        debug("got sth");
        if (sender.sin_addr.s_addr != server_addr.sin_addr.s_addr or sender.sin_port != server_addr.sin_port)
            continue;
        debug("msg from correct server");
        size_t start, len;
        int matched = sscanf(buffer, "DATA %lu %lu", &start, &len);
        if (matched != 2)
            continue;
        debug("matched!");
        if (start < bytes_received)
        {
            debug("already written to file");
            continue;
        }
        char *data_start = strchr(buffer, '\n') + 1;
        const auto empl_res = window.emplace(start, file_part(data_start, len));
        if (empl_res.second)
            debug("inserted!");
        else
            debug("already have it...");
        bytes_received = save_data(pFile, bytes_received);
    }
    return bytes_received;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        cout << "Wrong number of parameters.\nUsage: $ ./transport port output_file size_in_bytes\n";
        return EXIT_FAILURE;
    }
    int port = atoi(argv[1]);
    if (port > USHRT_MAX or port < 1)
    {
        cout << "Wrong port number\n";
        return EXIT_FAILURE;
    }
    string fname = string(argv[2]);
    FILE *pFile = fopen(fname.c_str(), "w");
    if (pFile == NULL)
    {
        cout << "Error opening file \"" + fname + "\"\n";
        return EXIT_FAILURE;
    }

    size_t total_len = atoi(argv[3]);

    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_aton("156.17.4.30", &server_addr.sin_addr);

    int fd = init(port);

    size_t bytes_received = 0;

    send_queries(fd, server_addr, bytes_received, total_len);
    send_queries(fd, server_addr, bytes_received, total_len);
    //send_queries(fd, server_addr, bytes_received, total_len);
    debug("started sleeping");
    //usleep(500000); // sleep for 0.5 sec
    sleep(1);
    debug("woke up!");
    while (bytes_received < total_len)
    {
        bytes_received = collect_responses(fd, server_addr, bytes_received, pFile);
        send_queries(fd, server_addr, bytes_received, total_len);
    }
    fclose(pFile);
    return EXIT_SUCCESS;
}