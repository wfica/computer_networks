// Wojciech Fica
// 280180

#ifndef UTILS_H
#define UTILS_H

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

#define MX_DISTANCE 20
#define BROADCAST_TIME 10
#define LISTENING_TIME 4 // time server waits for udp packets
#define PORT_NUMBER 54321
#define UNREACHABLE UINT32_MAX
#define MX_PERIODS_SINCE_AVAILABLE 3
#define DEBUG 0

using namespace std;

bool addr_belongs_to_net(in_addr_t addr, in_addr_t network_ip, unsigned int binary_mask);
void print_as_bytes (unsigned char* buff, ssize_t length);
string binary_ip_to_string(in_addr_t);
unsigned int binary_mask_from_uint8(uint8_t mask);
unsigned int mask_len(unsigned int binary_mask);
in_addr_t get_broadcast_address(in_addr_t ip_addr, unsigned int binary_mask );

struct network
{
    in_addr_t ip_addr;                           // 172.16.0.0
    unsigned int binary_mask;                    // 0xffff0000
    bool direct;                                 // true
    unsigned int distance;                       // 4
    unsigned int periods_passed_since_available; // 0

    in_addr_t first_hop; // 192.14.12.13

    void read_net();
    bool operator== (const network&) const;
};

ostream &operator<<(ostream &out, const network &net);

struct networks
{
    vector<in_addr_t> my_interfaces; 
    vector<unsigned int> cable_length;
    // i-th interface and i-th cable_length correspond to i-th network
    vector<network> nets;
    void read_nets();
};

ostream &operator<<(ostream &out, const networks &vec);


#endif