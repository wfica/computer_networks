// Wojciech Fica
// 280180

#include "send.h"

int send_udp(const network &info, const sockaddr_in &address, const int fd)
{
	char message[12];
	memcpy(message, &info.ip_addr, (size_t)4);
	uint8_t mask = (uint8_t)mask_len(info.binary_mask);
	memcpy(message + 4, &mask, (size_t)1);
	unsigned int big_endian_dist = __builtin_bswap32(info.distance);
	memcpy(message + 5, &big_endian_dist, (size_t)4);

	ssize_t message_len = 9;

	ssize_t sendto_res = sendto(fd, message, message_len, 0, (struct sockaddr *)&address, sizeof(address));
	if (DEBUG)
	{
		cout << sendto_res << " <- tyle wysłano ";
		fprintf(stderr, "sendto error: %s\n", strerror(errno));
	}

	if (sendto_res != message_len) // or errno == ENOTCONN or errno == ENETUNREACH)
	{
		if (DEBUG)
		{
			fprintf(stderr, "sendto error: %s\n", strerror(errno));
		}
		return 0;
	}
	return 1;
}

// returns number of packages successfully sent
int broadcast_udps(const in_addr_t net_ip_addr, const int fd, const networks &vec)
{
	int sent_count = 0;

	sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT_NUMBER);
	//inet_pton(AF_INET, binary_ip_to_string(net_ip_addr).c_str(), &address.sin_addr);
	address.sin_addr.s_addr = net_ip_addr;
	//	printf("%x\n", address.sin_addr.s_addr);

	// for (size_t i = 0; i < vec.my_interfaces.size(); ++i)
	// 	if (!(vec.nets[i].distance == UNREACHABLE and vec.nets[i].periods_passed_since_available > MX_PERIODS_SINCE_AVAILABLE))
	// 		sent_count += send_udp(vec.nets[i], address, fd);

	// for (size_t i = vec.my_interfaces.size(); i < vec.nets.size(); ++i)
	// 	sent_count += send_udp(vec.nets[i], address, fd);
	for (size_t i = 0; i < vec.nets.size(); ++i)
		sent_count += send_udp(vec.nets[i], address, fd);
	return sent_count;
}

void send_vec(int fd, networks &vec)
{
	for (size_t i = 0; i < vec.my_interfaces.size(); ++i)
	{
		// send vec content to a broadcast address
		in_addr_t br_addr = __builtin_bswap32(__builtin_bswap32(vec.my_interfaces[i]) | (~vec.nets[i].binary_mask));
		if (broadcast_udps(br_addr, fd, vec) == 0)
		{
			// nie udalo sie wyslac zadnego pakietu do sasiada
			if (DEBUG)
			{
				cout << "niedostepna siec " << i << "\n";
			}
			vec.nets[i].distance = UNREACHABLE;
		}
		else
		{
			// udalo się wysłać coś przez interfejs
			if (DEBUG)
			{
				cout << "udalo sie cos wyslac przez siec " << i << "\n";
				cout << "aktualy dist = " << vec.nets[i].distance << ", a dlugosc kabla = " << vec.cable_length[i] << endl;
			}
			vec.nets[i].periods_passed_since_available = 0;
			vec.nets[i].distance = min(vec.nets[i].distance, vec.cable_length[i]);
		}
	}
}