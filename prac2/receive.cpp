// Wojciech Fica
// 280180

#include "utils.h"

bool message_send_by_me(const network &info, const networks &vec)
{
	size_t n = vec.my_interfaces.size();
	for (size_t i = 0; i < n; ++i)
		if (info.first_hop == vec.my_interfaces[i])
			return true;
	return false;
}

unsigned int jump_distance(const network &info, const networks &vec)
{
	size_t n = vec.my_interfaces.size(); // number of directly connected networks
	for (size_t i = 0; i < n; ++i)
	{
		//printf("%x %x\n", vec.my_interfaces[i], info.first_hop);
		if (addr_belongs_to_net(info.first_hop, vec.nets[i].ip_addr, vec.nets[i].binary_mask))
		{
			if (message_send_by_me(info, vec))
				return 0;
			return vec.cable_length[i];
		}
	}
	return UNREACHABLE;
}

unsigned int total_distance(const network &info, const networks &vec)
{
	unsigned long long res = (unsigned long long)(jump_distance(info, vec)) +
							 (unsigned long long)(info.distance);
	if (res >= (unsigned long long)UNREACHABLE)
		return UNREACHABLE;
	return (unsigned int)res;
}

void check_incoming_entry(network &info, networks &vec)
{
	if (message_send_by_me(info, vec))
	{
		if (DEBUG)
		{
			cout << "msg sent by me\n";
		}
		return;
	}
	unsigned int total_dist = total_distance(info, vec);
	if (DEBUG)
	{
		cout << "total_dist= " << total_dist << "\n";
	}

	for (size_t i = 0; i < vec.nets.size(); ++i)
	{
		if (vec.nets[i].ip_addr == info.ip_addr and vec.nets[i].binary_mask == info.binary_mask)
		{
			if (DEBUG)
			{
				cout << "already connected net\n";
			}
			if (info.first_hop == vec.nets[i].first_hop)
			{
				vec.nets[i].distance = total_dist;
			}
			else if (vec.nets[i].distance > total_dist)
			{
				vec.nets[i].distance = total_dist;
				vec.nets[i].first_hop = info.first_hop;
			}
			return;
		}
	}

	if (DEBUG)
	{
		cout << "new net\n";
	}
	if (total_dist > MX_DISTANCE)
		return;
	info.distance = total_dist;
	vec.nets.push_back(info);
}

void mark_available(networks &vec, in_addr_t sender_ip)
{
	for (size_t i = 0; i < vec.my_interfaces.size(); ++i)
		if (addr_belongs_to_net(sender_ip, vec.nets[i].ip_addr, vec.nets[i].binary_mask))
		{
			vec.nets[i].periods_passed_since_available = 0;
			if (vec.nets[i].distance > vec.cable_length[i])
			{
				vec.nets[i].distance = vec.cable_length[i];
			}
		}
}

vector< pair<  network, in_addr_t> > receive(int fd, networks &vec)
{
	// creating fd_set
	fd_set descriptors;
	FD_ZERO(&descriptors);
	FD_SET(fd, &descriptors);
	timeval tv;
	tv.tv_sec = LISTENING_TIME;
	tv.tv_usec = 0;

	sockaddr_in sender;
	socklen_t sender_len = sizeof(sender);
	u_int8_t buffer[IP_MAXPACKET];

	network info;
	vector< pair<  network, in_addr_t> > senders;
	senders.resize(0);
	while (select(fd + 1, &descriptors, NULL, NULL, &tv) > 0)
	{
		ssize_t datagram_len = recvfrom(fd, buffer, IP_MAXPACKET, MSG_DONTWAIT,
										(struct sockaddr *)&sender, &sender_len);
		if (datagram_len < 0)
		{
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno));
			return senders;
		}
		//char sender_ip_str[20];
		//inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str));
		//printf("Received UDP packet from IP address: %s, port: %d\n",
		//	   sender_ip_str, ntohs(sender.sin_port));

		//buffer[datagram_len] = 0;
		//printf("%ld-byte message: +%s+\n", datagram_len, buffer);
		memcpy(&info.ip_addr, buffer, 4);
		uint8_t mask;
		memcpy(&mask, buffer + 4, 1);
		info.binary_mask = binary_mask_from_uint8(mask);
		unsigned int big_endian_dist;
		memcpy(&big_endian_dist, buffer + 5, 4);
		info.distance = __builtin_bswap32(big_endian_dist);
		info.first_hop = sender.sin_addr.s_addr;
		mark_available(vec, sender.sin_addr.s_addr);
		senders.push_back(make_pair(info, sender.sin_addr.s_addr));
		info.direct = false;
		info.periods_passed_since_available = 0;
		if (DEBUG)
		{
			cout << "received info: " << info << "\n";
		}
		check_incoming_entry(info, vec);
	}
	return senders;
}