#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // for int64_t
#include <unistd.h>
#include <pthread.h>
#include "unp.h"

pthread_mutex_t file_mutex;

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    free(arg);
    char buffer[BUFFSIZE];
    char file_name[BUFFSIZE];
    int64_t file_size;

    // 接收檔案名稱
    memset(file_name, 0, sizeof(file_name));
    int bytes_received = recv(client_socket, file_name, sizeof(file_name) - 1, 0);
    if (bytes_received <= 0) {
        perror("Failed to receive file name");
        close(client_socket);
        pthread_exit(NULL);
    }
    file_name[bytes_received] = '\0';
    char *newline = strchr(file_name, '\n');
    if (newline) *newline = '\0';
    newline = strchr(file_name, '\r');
    if (newline) *newline = '\0';

    printf("Receiving file: %s\n", file_name);

    // 使用互斥鎖保護檔案操作
    pthread_mutex_lock(&file_mutex);

    // 檢查檔案是否為 .txt
    const char *ext = strrchr(file_name, '.');
    if (!ext || strcmp(ext, ".txt") != 0) {
        const char *error_msg = "Error: Only .txt files are allowed";
        send(client_socket, error_msg, strlen(error_msg), 0);
        printf("Rejected file '%s': Not a .txt file.\n", file_name);
        pthread_mutex_unlock(&file_mutex);
        close(client_socket);
        pthread_exit(NULL);
    }

    // 檢查檔案是否已存在
    char save_path[BUFFSIZE];
    snprintf(save_path, sizeof(save_path), "Questions/%s", file_name);
    if (access(save_path, F_OK) == 0) { // 檔案已存在
        const char *error_msg = "Error: File with the same name already exists";
        send(client_socket, error_msg, strlen(error_msg), 0);
        printf("File '%s' already exists. Rejecting upload.\n", file_name);
        pthread_mutex_unlock(&file_mutex);
        close(client_socket);
        pthread_exit(NULL);
    }

    pthread_mutex_unlock(&file_mutex); // 解鎖檔案操作結束

    // 接收檔案大小
    int size_received = recv(client_socket, &file_size, sizeof(file_size), MSG_WAITALL);
    if (size_received != sizeof(file_size)) {
        perror("Failed to receive file size");
        close(client_socket);
        pthread_exit(NULL);
    }
    printf("File size received: %ld bytes\n", file_size);

    // 打開檔案以存儲接收到的內容
    FILE *file = fopen(save_path, "wb");
    if (!file) {
        perror("Failed to create file");
        close(client_socket);
        pthread_exit(NULL);
    }

    // 接收檔案內容
    printf("Receiving file content...\n");
    long total_received = 0;
    while (total_received < file_size) {
        bytes_received = recv(client_socket, buffer, BUFFSIZE, 0);
        if (bytes_received < 0) {
            perror("Failed to receive file content");
            break;
        } else if (bytes_received == 0) {
            printf("Client closed the connection prematurely\n");
            break;
        }
        fwrite(buffer, 1, bytes_received, file);
        total_received += bytes_received;
        printf("Progress: %ld/%ld bytes\n", total_received, file_size);
    }

    fclose(file);

    if (total_received == file_size) {
        printf("File received successfully and saved as '%s'\n", save_path);
        const char *response = "File received successfully";
        send(client_socket, response, strlen(response), 0);
    } else {
        printf("Incomplete file received for '%s'.\n", save_path);
    }

    close(client_socket);
    pthread_exit(NULL);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // 初始化互斥鎖
    if (pthread_mutex_init(&file_mutex, NULL) != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }

    // 建立 Socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERV_PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        pthread_mutex_destroy(&file_mutex);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        pthread_mutex_destroy(&file_mutex);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", SERV_PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        printf("Client connected\n");

        int *client_sock = malloc(sizeof(int));
        *client_sock = client_socket;

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_sock) != 0) {
            perror("Thread creation failed");
            free(client_sock);
            continue;
        }

        pthread_detach(thread_id);
    }

    close(server_socket);
    pthread_mutex_destroy(&file_mutex);
    return 0;
}
