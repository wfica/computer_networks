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

using namespace std;

bool addr_belongs_to_net(in_addr_t addr, in_addr_t network_ip, unsigned int binary_mask)
{
	return (__builtin_bswap32(__builtin_bswap32(addr) & binary_mask) == network_ip);
}


void print_as_bytes (unsigned char* buff, ssize_t length)
{
	for (ssize_t i = 0; i < length; i++, buff++)
		printf ("%.2x ", *buff);	
}

unsigned int binary_mask_from_uint8(uint8_t mask)
{
    if (mask == 0)
        return 0;
    if (mask == 32)
        return UINT32_MAX;
    return (~((1 << (32 - mask)) - 1));
}

unsigned int mask_len(unsigned int binary_mask)
{
    return __builtin_popcount(binary_mask);
}

in_addr_t get_broadcast_address(in_addr_t ip_addr, unsigned int binary_mask)
{
    return __builtin_bswap32(__builtin_bswap32(ip_addr) | (~binary_mask));
}

void network::read_net()
{
    string ip_and_mask, tmp;
    cin >> ip_and_mask >> tmp >> distance;
    ip_addr = inet_addr(ip_and_mask.substr(0, ip_and_mask.find('/')).c_str());
    binary_mask = binary_mask_from_uint8(stoi(ip_and_mask.substr(ip_and_mask.find('/') + 1)));
    direct = true;
    periods_passed_since_available = MX_PERIODS_SINCE_AVAILABLE + 1;
    first_hop = ip_addr;
}

bool network::operator== (const network &other) const{
    return (other.ip_addr == ip_addr && other.binary_mask == binary_mask);
}

string binary_ip_to_string(in_addr_t ip)
{
    char buffer[20];
    inet_ntop(AF_INET, &ip, buffer, sizeof(buffer));
    return string(buffer);
}

ostream &operator<<(ostream &out, const network &net)
{
    out << binary_ip_to_string(net.ip_addr) << '/'
        << mask_len(net.binary_mask) << " ";
    if (net.distance <= MX_DISTANCE)
        out << "distance " << net.distance << " ";
    else
        out << "unreachable ";
    if (net.direct)
        out << "connected directly";
    else
        out << "via " << binary_ip_to_string(net.first_hop) ;
    if(DEBUG) out << "("<< net.periods_passed_since_available << ")";
    return out;
}

void networks::read_nets()
{
    int n;
    cin >> n;
    nets.resize(n);
    cable_length.resize(n);
    my_interfaces.resize(n);
    for (int i = 0; i < n; ++i)
    {
        nets[i].read_net();
        my_interfaces[i] = nets[i].ip_addr;
        cable_length[i] = nets[i].distance;
        nets[i].ip_addr = __builtin_bswap32(__builtin_bswap32(nets[i].ip_addr) & nets[i].binary_mask);
    }
}

ostream &operator<<(ostream &out, const networks &vec)
{
    for (const auto &net : vec.nets)
        out << net << "\n";
    return out;
}
