// Wojciech Fica
// 280180


#ifndef SENDING_H
#define SENDING_H

bool create_recipient(const char *target_ip, struct sockaddr_in *recipient);
bool send_package(const int sockfd, const int seq, const int ttl,
                  const struct sockaddr_in *recipient, struct three_pck *replies);

#endif