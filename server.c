#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include	"unp.h"


void handle_client(int connfd) {
    char buffer[MAXLINE];
    ssize_t n;

    while ((n = read(connfd, buffer, MAXLINE)) > 0) {
        buffer[n] = '\0';
        printf("Received: %s", buffer);

        // 回應客戶端
        char response[] = "Message received!\n";
        write(connfd, response, strlen(response));
    }

    if (n < 0) {
        perror("Read error");
    }

    close(connfd);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int listenfd, connfd;
    struct sockaddr_in servaddr;

    // 創建套接字
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 配置伺服器地址
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 接收任意地址的請求
    servaddr.sin_port = htons(atoi(argv[1]));    // 轉換端口

    // 綁定地址
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 開始監聽
    if (listen(listenfd, LISTENQ) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %s...\n", argv[1]);

    // 接受客戶端連接
    while (1) {
        connfd = accept(listenfd, (struct sockaddr *)NULL, NULL);
        if (connfd < 0) {
            perror("Accept failed");
            continue;
        }
        printf("Client connected!\n");

        // 處理客戶端請求
        handle_client(connfd);
    }

    close(listenfd);
    return 0;
}
