#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include "unp.h"

int connect_to_server(const char *server_ip, int server_port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection to server failed");
        exit(EXIT_FAILURE);
    }

    return sock; // 返回 socket 描述符
}

void send_to_server(int sock, const char *message) {
    send(sock, message, strlen(message), 0);
}

void receive_from_server(int sock, char *buffer, size_t buffer_size) {
    int len = recv(sock, buffer, buffer_size, 0);
    if (len > 0) {
        buffer[len] = '\0'; // 確保字串結束
    } else {
        buffer[0] = '\0';
    }
}
