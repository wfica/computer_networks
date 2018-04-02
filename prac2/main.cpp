// Wojciech Fica
// 280180

#include <bits/stdc++.h>
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
#include "receive.h"
#include "send.h"

using namespace std;

// server - waits for udp packet
// client - sends upd packet

int init_server()
{
	int bcast_permission = 1;
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0)
	{
		fprintf(stderr, "socket error: %s\n", strerror(errno));
		return -1;
	}

	sockaddr_in server_address;
	bzero(&server_address, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(PORT_NUMBER);
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	{
		fprintf(stderr, "bind error: %s\n", strerror(errno));
		return -1;
	}
	setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &bcast_permission, sizeof(bcast_permission));
	return fd;
}

void remove_unreachable(networks &vec)
{
	for (size_t i = vec.my_interfaces.size(); i < vec.nets.size(); ++i)
	{
		if (DEBUG)
		{
			cout << "checking for potential removal: pp=" << vec.nets[i].periods_passed_since_available << "  dist=" << vec.nets[i].distance << "\n";
		}
		if (vec.nets[i].periods_passed_since_available > MX_PERIODS_SINCE_AVAILABLE and
			vec.nets[i].distance > MX_DISTANCE)
		{
			swap(vec.nets[i], vec.nets.back());
			vec.nets.pop_back();
			i--;
		}
	}
}

int main()
{
	networks vec;
	int fd;
	vec.read_nets();

	fd = init_server();

	while (true)
	{
		cout << vec << "\n";
		remove_unreachable(vec);

		for (auto &net : vec.nets)
			net.periods_passed_since_available++;

		sleep(BROADCAST_TIME);
		send_vec(fd, vec);
		vector< pair<  network, in_addr_t> > senders = receive(fd, vec);

		for (size_t i = 0; i < vec.my_interfaces.size(); ++i)
			if (vec.nets[i].periods_passed_since_available > MX_PERIODS_SINCE_AVAILABLE)
				vec.nets[i].distance = UNREACHABLE;
		for (size_t i = vec.my_interfaces.size(); i < vec.nets.size(); ++i)
		{
			const unsigned int dist = vec.nets[i].distance;
			vec.nets[i].distance = UNREACHABLE;
			for (size_t j =0 ; j< senders.size(); ++j)
				if (vec.nets[i].first_hop == senders[j].second and vec.nets[i].ip_addr == senders[j].first.ip_addr)
					vec.nets[i].distance = dist;
		}
	}
	return EXIT_SUCCESS;
}