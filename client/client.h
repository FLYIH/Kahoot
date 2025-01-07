#ifndef CLIENT_H
#define CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "unp.h"
#include <stdlib.h>
// Connect to server and return socket file descriptor
int connect_to_server(const char *ip_address);
//void sendA(int sockfd);
void send_name(int sockfd, const char *name);
void readID(int sockfd);
// Send a message to the server
void send_and_receive(int sockfd, const char *message, char *response, size_t response_size);

// Close the connection
void close_connection(int sockfd);

#ifdef __cplusplus
}
#endif

#endif
    