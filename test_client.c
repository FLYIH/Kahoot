#include "unp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    int sockfd, n;
    char recvline[MAXLINE], sendline[MAXLINE], username[20];
    struct sockaddr_in servaddr;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Server IP>\n", argv[0]);
        exit(1);
    }

    sockfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT + 4); // Match the server's port
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    Connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0'; // Remove trailing newline
    Writen(sockfd, username, strlen(username));

    if ((n = Read(sockfd, recvline, MAXLINE)) > 0) {
        recvline[n] = '\0';
        printf("Server: %s\n", recvline);
    }

    fd_set rset;
    FD_ZERO(&rset);

    while (1) {
        FD_SET(fileno(stdin), &rset);
        FD_SET(sockfd, &rset);
        int maxfdp1 = max(fileno(stdin), sockfd) + 1;

        Select(maxfdp1, &rset, NULL, NULL, NULL);

        if (FD_ISSET(fileno(stdin), &rset)) { // Input from user
            if (fgets(sendline, MAXLINE, stdin) == NULL) {
                break; // EOF (Ctrl+D)
            }
            Writen(sockfd, sendline, strlen(sendline));
        }

        if (FD_ISSET(sockfd, &rset)) { // Message from server
            if ((n = Read(sockfd, recvline, MAXLINE)) == 0) {
                printf("Server closed the connection.\n");
                break;
            }
            recvline[n] = '\0';
            printf("Server: %s\n", recvline);
        }
    }

    Close(sockfd);
    return 0;
}
