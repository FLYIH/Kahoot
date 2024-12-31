#include "unp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];
    fd_set readfds;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <ServerIP> <Name>\n", argv[0]);
        exit(1);
    }

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

    snprintf(sendline, sizeof(sendline), "%s\n", argv[2]);
    Writen(sockfd, sendline, strlen(sendline));

    // 設置標準輸入為非阻塞模式
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int maxfd = max(sockfd, STDIN_FILENO) + 1;
        select(maxfd, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(sockfd, &readfds)) {
            if (Readline(sockfd, recvline, MAXLINE) > 0) {
                Fputs(recvline, stdout);
            } else {
                break; // 伺服器關閉連接
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (Fgets(sendline, MAXLINE, stdin) != NULL) {
                Writen(sockfd, sendline, strlen(sendline));
            }
        }
    }

    Close(sockfd);
    exit(0);
}
