// Wojciech Fica
// 280180

#ifndef SEND_H
#define SEND_H

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


int send_udp(const network &info, const sockaddr_in &address, const int fd);

// returns number of packages successfully sent
int broadcast_udps(const in_addr_t net_ip_addr, const int fd, const networks &vec);

void send_vec(int fd, networks &vec);

#endif