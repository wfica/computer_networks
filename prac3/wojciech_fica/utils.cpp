// Wojciech Fica
// 280180

#include "utils.h"

using namespace std;

file_part::file_part(char *data_start, size_t length)
{
    len = length;
    memcpy(buffer, data_start, length);
}

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