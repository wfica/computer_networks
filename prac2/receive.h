// Wojciech Fica
// 280180

#ifndef RECEIVE_H
#define RECEIVE_H

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

vector< pair<  network, in_addr_t> >  receive(int fd, networks &vec);

#endif