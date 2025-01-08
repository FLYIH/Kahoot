#include "test_client.h"
#include <iostream>
#include <string>
#include "client.h"

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(_WIN32) || defined(__LITTLE_ENDIAN__)
    #define htobe64(x) __builtin_bswap64(x)
    #define be64toh(x) __builtin_bswap64(x)
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
      defined(__BIG_ENDIAN__)
    #define htobe64(x) (x)
    #define be64toh(x) (x)
#else
    #error "Unknown byte order"
#endif

void send_file(int sockfd, const char *file_path) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'.\n", file_path);
        return;
    }

    const char *file_name = strrchr(file_path, '/');
    file_name = file_name ? file_name + 1 : file_path;
    char sendline[MAXLINE];
    snprintf(sendline, sizeof(sendline), "UPLOAD %s\n", file_name);
    Writen(sockfd, sendline, strlen(sendline));

    fseek(file, 0, SEEK_END);
    int64_t file_size = ftell(file);
    rewind(file);

    int64_t net_file_size = htobe64(file_size);
    Writen(sockfd, &net_file_size, sizeof(net_file_size));
    printf("File size sent: %ld bytes\n", file_size);

    char buffer[MAXLINE];
    size_t bytes_read, total_sent = 0;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        Writen(sockfd, buffer, bytes_read);
        total_sent += bytes_read;
        printf("Progress: %zu/%ld bytes\n", total_sent, file_size);
    }

    fclose(file);
    printf("File '%s' sent successfully.\n", file_name);

    if (Readline(sockfd, buffer, MAXLINE) > 0) {
        printf("Server response: %s", buffer);
    }
}

void run_test_client(int sockfd, const char *server_ip, const char *name) {
    char sendline[MAXLINE], recvline[MAXLINE];
    fd_set readfds;

    snprintf(sendline, sizeof(sendline), "%s\n", name);
    Writen(sockfd, sendline, strlen(sendline));

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
            } else  {
                break;
            }
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (Fgets(sendline, MAXLINE, stdin) != NULL) {
                /*if (strncmp(sendline, "UPLOAD ", 7) == 0) {
                    char *file_path = sendline + 7;
                    file_path[strcspn(file_path, "\n")] = '\0';
                    send_file(sockfd, file_path);
                } else {
                    Writen(sockfd, sendline, strlen(sendline));
                }*/
                sendline[strcspn(sendline, "\n")] = '\0';
                send_file(sockfd, sendline);
            }
        }
    }
}