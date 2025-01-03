#include "unp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ncurses.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <endian.h>

#include <stdint.h>

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

#define MAX_FILES 1024
#define PATH_MAX 4096
#define SERVER_IP "127.0.0.1"

// 過濾條件，只顯示 .txt 文件和目錄
int txt_filter(const struct dirent *entry) {
    if (entry->d_name[0] == '.') return 0; // 忽略隱藏文件
    if (entry->d_type == DT_DIR) return 1; // 保留目錄
    const char *ext = strrchr(entry->d_name, '.');
    return ext && strcmp(ext, ".txt") == 0;
}

// 文件選擇器
void file_selector(char *selected_file) {
    char cwd[PATH_MAX];
    struct dirent **file_list;
    int num_files, highlight = 0;

    while (1) {
        if (!getcwd(cwd, sizeof(cwd))) {
            perror("getcwd");
            exit(EXIT_FAILURE);
        }

        // 過濾 .txt 文件和目錄
        num_files = scandir(cwd, &file_list, txt_filter, alphasort);
        if (num_files < 0) {
            perror("scandir");
            exit(EXIT_FAILURE);
        }

        initscr();
        clear();
        noecho();
        cbreak();
        keypad(stdscr, TRUE);

        mvprintw(0, 0, "Current directory: %s", cwd);
        mvprintw(1, 0, "Use UP/DOWN arrows to navigate. Press ENTER to select. '..' to go up.");

        for (int i = 0; i < num_files; i++) {
            if (i == highlight) attron(A_REVERSE);
            mvprintw(3 + i, 0, "%s", file_list[i]->d_name);
            if (i == highlight) attroff(A_REVERSE);
        }

        int key = getch();
        switch (key) {
            case KEY_UP:
                highlight = (highlight - 1 + num_files) % num_files;
                break;
            case KEY_DOWN:
                highlight = (highlight + 1) % num_files;
                break;
            case 10: { // Enter 鍵
                struct stat file_stat;
                char selected_path[PATH_MAX];
                snprintf(selected_path, PATH_MAX, "%s/%s", cwd, file_list[highlight]->d_name);

                if (stat(selected_path, &file_stat) == 0 && S_ISDIR(file_stat.st_mode)) {
                    chdir(selected_path); // 進入目錄
                    for (int i = 0; i < num_files; i++) free(file_list[i]);
                    free(file_list);
                    endwin();
                    continue;
                } else {
                    strncpy(selected_file, selected_path, PATH_MAX);
                    goto end_selection;
                }
            }
        }
    }

end_selection:
    for (int i = 0; i < num_files; i++) free(file_list[i]);
    free(file_list);
    endwin();
}

// // 傳送檔案到伺服器
// void send_file_to_server(const char *file_path, int sock) {
//     FILE *file = fopen(file_path, "rb");
//     if (!file) {
//         fprintf(stderr, "Error: File '%s' not found.\n", file_path);
//         return;
//     }

//     // 傳送檔案名稱
//     const char *file_name = strrchr(file_path, '/');
//     file_name = file_name ? file_name + 1 : file_path;
//     if (send(sock, file_name, strlen(file_name) + 1, 0) < 0) {
//         perror("Failed to send file name");
//         fclose(file);
//         return;
//     }
//     printf("File name sent: %s\n", file_name);

//     // 傳送檔案大小
//     fseek(file, 0, SEEK_END);
//     int64_t file_size = ftell(file);
//     rewind(file);
//     if (send(sock, &file_size, sizeof(file_size), 0) < 0) {
//         perror("Failed to send file size");
//         fclose(file);
//         return;
//     }
//     printf("File size sent: %ld bytes\n", file_size);

//     // 傳送檔案內容
//     char buffer[BUFFSIZE];
//     size_t bytes_read;
//     long total_sent = 0;
//     while ((bytes_read = fread(buffer, 1, BUFFSIZE, file)) > 0) {
//         if (send(sock, buffer, bytes_read, 0) < 0) {
//             perror("Failed to send file content");
//             fclose(file);
//             return;
//         }
//         total_sent += bytes_read;
//         printf("Progress: %ld/%ld bytes\n", total_sent, file_size);
//     }
//     fclose(file);
//     printf("File sent successfully. Total sent: %ld bytes\n", total_sent);

//     // 接收伺服器回應
//     char response[BUFFSIZE];
//     int bytes_received = recv(sock, response, sizeof(response) - 1, 0);
//     if (bytes_received > 0) {
//         response[bytes_received] = '\0';
//         printf("Server response: %s\n", response);
//     } else {
//         perror("Failed to receive server response");
//     }
// }


void send_file_to_server(const char *file_path, int sock) {
    FILE *file = fopen(file_path, "rb");
    if (!file) {
        fprintf(stderr, "Error: File '%s' not found.\n", file_path);
        return;
    }

    // 傳送準備訊號
    const char *upload_signal = "UploadClient";
    if (send(sock, upload_signal, strlen(upload_signal) + 1, 0) < 0) {
        perror("Failed to send upload signal");
        fclose(file);
        return;
    }

    char server_response[BUFFSIZE];
    int bytes_received = recv(sock, server_response, sizeof(server_response) - 1, 0);
    if (bytes_received <= 0) {
        perror("Server did not respond to upload signal");
        fclose(file);
        return;
    }
    server_response[bytes_received] = '\0';
    if (strcmp(server_response, "Ready") != 0) {
        fprintf(stderr, "Server is not ready: %s\n", server_response);
        fclose(file);
        return;
    }

    // 傳送檔案名稱
    const char *file_name = strrchr(file_path, '/');
    file_name = file_name ? file_name + 1 : file_path;
    uint16_t file_name_len = htons(strlen(file_name) + 1); // 加 1 包含 '\0'
    if (send(sock, &file_name_len, sizeof(file_name_len), 0) < 0 || 
        send(sock, file_name, strlen(file_name) + 1, 0) < 0) {
        perror("Failed to send file name");
        fclose(file);
        return;
    }
    printf("File name sent: %s\n", file_name);

    // 傳送檔案大小
    fseek(file, 0, SEEK_END);
    int64_t file_size = ftell(file);
    rewind(file);
    int64_t net_file_size = htobe64(file_size); // 序列化為大端
    if (send(sock, &net_file_size, sizeof(net_file_size), 0) < 0) {
        perror("Failed to send file size");
        fclose(file);
        return;
    }
    printf("File size sent: %ld bytes\n", file_size);

    // 傳送檔案內容
    char buffer[BUFFSIZE];
    size_t bytes_read;
    long total_sent = 0;
    while ((bytes_read = fread(buffer, 1, BUFFSIZE, file)) > 0) {
        if (send(sock, buffer, bytes_read, 0) < 0) {
            perror("Failed to send file content");
            fclose(file);
            return;
        }
        total_sent += bytes_read;
        printf("Progress: %ld/%ld bytes\n", total_sent, file_size);
    }
    fclose(file);
    printf("File sent successfully. Total sent: %ld bytes\n", total_sent);

    // 接收伺服器回應
    bytes_received = recv(sock, server_response, sizeof(server_response) - 1, 0);
    if (bytes_received > 0) {
        server_response[bytes_received] = '\0';
        printf("Server response: %s\n", server_response);
    } else {
        perror("Failed to receive server response");
    }
}


// 主程式
int main() {
    char file_path[PATH_MAX];
    file_selector(file_path);
    if (strlen(file_path) == 0) {
        printf("No file selected.\n");
        return 0;
    }

    int sock;
    struct sockaddr_in serv_addr;

    // 建立 Socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 設定伺服器地址
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERV_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        exit(EXIT_FAILURE);
    }

    // 連接伺服器
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // 傳送檔案
    send_file_to_server(file_path, sock);

    close(sock);
    return 0;
}