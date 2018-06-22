// Wojciech Fica
// 280180

#include <bits/stdc++.h>

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <unistd.h>

#include "utils.h"

using namespace std;

unordered_map<size_t, file_part> window;

size_t save_data(FILE *pFile, size_t bytes_received)
{
    unordered_map<size_t, file_part>::iterator it = window.find(bytes_received);
    while (it != window.end())
    {
        if (fwrite(it->second.buffer, sizeof(char), it->second.len, pFile) != it->second.len)
            printf("fwrite error\n");
        bytes_received += it->second.len;
        window.erase(it);
        it = window.find(bytes_received);
    }
//    cout << bytes_received << "\n";
    return bytes_received;
}

size_t collect_responses(const int fd, const sockaddr_in server_addr, size_t bytes_received, FILE *pFile)
{
    char buffer[IP_MAXPACKET];
    sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(fd, &descriptors);
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 25000;
    while (select(fd + 1, &descriptors, NULL, NULL, &tv))
    {
        while (recvfrom(fd, buffer, IP_MAXPACKET, MSG_DONTWAIT, (struct sockaddr *)&sender, &sender_len) > 0)
        {
            if (sender.sin_addr.s_addr != server_addr.sin_addr.s_addr or sender.sin_port != server_addr.sin_port)
                continue;
            size_t start, len;
            int matched = sscanf(buffer, "DATA %lu %lu", &start, &len);
            if (matched != 2)
                continue;
            if (start < bytes_received)
                continue;

            char *data_start = strchr(buffer, '\n') + 1;
            const auto empl_res = window.emplace(start, file_part(data_start, len));
            if (empl_res.second)
                bytes_received = save_data(pFile, bytes_received);
        }
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
    if (fd == -1)
        return EXIT_FAILURE;

    size_t bytes_received = 0;

    send_queries(fd, server_addr, bytes_received, total_len);
    send_queries(fd, server_addr, bytes_received, total_len);
    sleep(1);
    while (bytes_received < total_len)
    {
        bytes_received = collect_responses(fd, server_addr, bytes_received, pFile);
        send_queries(fd, server_addr, bytes_received, total_len);
    }
    fclose(pFile);
    return EXIT_SUCCESS;
}

/*
#!/bin/bash

PORT=40003

cd wojciech_fica
make

{ /usr/bin/time -v ./transport $PORT out $1 > /dev/null ; } 2> ../my_time 
mv out ../
cd ../
echo "my stat"
grep "Elapsed" my_time
grep "Maximum resident set size" my_time

{ /usr/bin/time -v ./transport-faster $PORT out_ref $1 > /dev/null ; } 2> mbi_time


echo "transport-faster stat"
grep "Elapsed" mbi_time
grep "Maximum resident set size" mbi_time

diff --brief out out_ref
rm out out_ref mbi_time my_time
*/
