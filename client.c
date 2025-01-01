#include "client.h"

int id = 0;
char sendline[MAXLINE];
char recvline[MAXLINE];

int connect_to_server(const char *ip_address) {
    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, ip_address, &servaddr.sin_addr);

    Connect(sockfd, (SA *)&servaddr, sizeof(servaddr));
    return sockfd;
}


void send_name(int sockfd, const char *name) {
    // 格式化消息
    snprintf(sendline, sizeof(sendline), "%s\n", name);
    Writen(sockfd, sendline, strlen(sendline));
}

void readID(int sockfd) {
    if (Readline(sockfd, recvline, MAXLINE) > 0) {
        id = atoi(recvline);
        printf("id = %d\n", id);
    }
}

void send_and_receive(int sockfd, const char *message, char *response, size_t response_size) {
    char sendline[MAXLINE];

    // 发送消息
    snprintf(sendline, sizeof(sendline), "%s\n", message);
    Write(sockfd, sendline, strlen(sendline));

    // 接收服务器响应
    ssize_t n = Readline(sockfd, response, response_size);
    if (n == 0) {
        err_quit("Server terminated prematurely");
    }

    response[n] = '\0'; // 确保字符串以空字符结束
}

void close_connection(int sockfd) {
    Close(sockfd);
}
