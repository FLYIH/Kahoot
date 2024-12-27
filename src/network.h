#ifndef NETWORK_H
#define NETWORK_H

#ifdef __cplusplus
extern "C" {
#endif

int connect_to_server(const char *server_ip, int server_port);
void send_to_server(int sock, const char *message);
void receive_from_server(int sock, char *buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif
