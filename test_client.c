#include "unp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[MAXLINE], recvline[MAXLINE];

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

    while (Readline(sockfd, recvline, MAXLINE) > 0) {
        Fputs(recvline, stdout);
        if (strncmp(recvline, "answer", 6) == 0) {
            printf("Enter your answer: ");
            if (Fgets(sendline, MAXLINE, stdin) != NULL) {
                Writen(sockfd, sendline, strlen(sendline));
            }
        }
    }

    Close(sockfd);
    exit(0);
}
