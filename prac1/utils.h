// Wojciech Fica
// 280180

#include <arpa/inet.h>
#include <stdbool.h>

u_int16_t compute_icmp_checksum(const void *buff, int length);
void print_as_bytes(unsigned char *buff, ssize_t length);
bool check_input(int argc, char *argv[]);