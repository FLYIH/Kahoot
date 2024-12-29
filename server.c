#include "unp.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_CHATROOMS 10
#define MAX_USERS_PER_ROOM 4

struct ChatRoom {
    int user_count;
    int connfds[MAX_USERS_PER_ROOM];
    char usernames[MAX_USERS_PER_ROOM][20];
};

struct ChatRoom chatrooms[MAX_CHATROOMS];

void sig_chld(int signo) {
    pid_t pid;
    int stat;
    while ((pid = waitpid(-1, &stat, WNOHANG)) > 0);
    return;
}

int find_available_room() {
    for (int i = 0; i < MAX_CHATROOMS; i++) {
        if (chatrooms[i].user_count < MAX_USERS_PER_ROOM)
            return i;
    }
    return -1;
}

void broadcast_to_room(int room_id, const char *message, int exclude_fd) {
    printf("Broadcasting in room %d: %s", room_id, message); // 調試輸出
    for (int i = 0; i < chatrooms[room_id].user_count; i++) {
        if (chatrooms[room_id].connfds[i] != exclude_fd) {
            Writen(chatrooms[room_id].connfds[i], message, strlen(message));
        }
    }
}


int main(int argc, char **argv) {
    int listenfd, connfd, maxfdp1;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    fd_set rset;
    char recvline[MAXLINE], sendline[MAXLINE];
    
    listenfd = Socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT + 4);
    Bind(listenfd, (SA *)&servaddr, sizeof(servaddr));
    Listen(listenfd, LISTENQ);
    Signal(SIGCHLD, sig_chld);

    printf("Server started. Listening on port %d\n", SERV_PORT + 4);

    // Initialize chatrooms
    for (int i = 0; i < MAX_CHATROOMS; i++) {
        chatrooms[i].user_count = 0;
    }

    while (1) {
        clilen = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (SA *)&cliaddr, &clilen)) < 0) {
            if (errno == EINTR)
                continue;
            else
                err_sys("accept error");
        }

        printf("New connection from %s:%d\n",
               Inet_ntop(AF_INET, &cliaddr.sin_addr, recvline, sizeof(recvline)),
               ntohs(cliaddr.sin_port));

        int room_id = find_available_room();
        if (room_id == -1) {
            snprintf(sendline, sizeof(sendline), "All chatrooms are full. Try again later.\n");
            Writen(connfd, sendline, strlen(sendline));
            Close(connfd);
            continue;
        }

        struct ChatRoom *room = &chatrooms[room_id];
        int user_index = room->user_count;
        room->connfds[user_index] = connfd;
        room->user_count++;

        // Read username
        int n = Read(connfd, room->usernames[user_index], sizeof(room->usernames[user_index]) - 1);
        room->usernames[user_index][n] = '\0';

        snprintf(sendline, sizeof(sendline), "Welcome to chatroom %d, %s!\n", room_id, room->usernames[user_index]);
        Writen(connfd, sendline, strlen(sendline));

        snprintf(sendline, sizeof(sendline), "%s has joined the chatroom.\n", room->usernames[user_index]);
        broadcast_to_room(room_id, sendline, connfd);

        if (Fork() == 0) {
            Close(listenfd);
            FD_ZERO(&rset);

            while (1) {
                FD_SET(connfd, &rset);
                maxfdp1 = connfd + 1;
                Select(maxfdp1, &rset, NULL, NULL, NULL);

                if (FD_ISSET(connfd, &rset)) {
                    int n = Read(connfd, recvline, MAXLINE);
                    if (n <= 0) {
                        printf("User %s disconnected from chatroom %d.\n", room->usernames[user_index], room_id);
                        snprintf(sendline, sizeof(sendline), "%s has left the chatroom.\n", room->usernames[user_index]);
                        broadcast_to_room(room_id, sendline, connfd);
                        Close(connfd);

                        // Remove user from room
                        for (int i = user_index; i < room->user_count - 1; i++) {
                            room->connfds[i] = room->connfds[i + 1];
                            strncpy(room->usernames[i], room->usernames[i + 1], sizeof(room->usernames[i]));
                        }
                        room->user_count--;
                        exit(0);
                    }

                    recvline[n] = '\0';
                    snprintf(sendline, sizeof(sendline), "%s: %s", room->usernames[user_index], recvline);
                    printf("Received from user %s in room %d: %s", room->usernames[user_index], room_id, recvline);
                    broadcast_to_room(room_id, sendline, connfd);
                }
            }
        }

        Close(connfd);
    }

    return 0;
}
