#include "unp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <Name>\n", argv[0]);
        exit(1);
    }

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    Inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));

    snprintf(sendline, sizeof(sendline), "%s\n", argv[1]);
    Writen(sockfd, sendline, strlen(sendline));

    while (Readline(sockfd, recvline, MAXLINE) > 0) {
        Fputs(recvline, stdout);
    }

    Close(sockfd);
    exit(0);
}
