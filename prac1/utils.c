// Wojciech Fica
// 280180

#include <arpa/inet.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#include "utils.h"

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

void print_as_bytes(unsigned char *buff, ssize_t length)
{
	for (ssize_t i = 0; i < length; i++, buff++)
		printf("%.2x ", *buff);
}

bool check_input(int argc, char *argv[])
{
	// checking the number of the arguments
	if (argc != 2)
	{
		printf("Invalid number of arguments.\nUsage: ./traceroute ip_address\n");
		return false;
	}

	// validating IP address
	in_addr_t ip_addr = inet_addr(argv[1]);
	if (ip_addr == (in_addr_t)(-1))
	{
		printf("Invalid IP addres.\nUsage: ./traceroute ip_address\n");
		return false;
	}
	return true;
}
