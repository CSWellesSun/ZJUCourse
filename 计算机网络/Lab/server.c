#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define MAX_QUEUE 10
#define BUF_SIZE 2048
#define ARG_NUM 3

struct client {
    int id; // 0 means invalid
    struct in_addr ip;
    int port;
    int fd;
};

int cur_client_num = 0;
struct client clients[MAX_QUEUE];

static int ParseInput(char* input, char** args) {
    // trim '\n'
    char *p = strchr(input, '\n');
    if (p != NULL) {
        *p = '\0';
    }
    
    // parse
    p = strchr(input, ' ');
    if (p == NULL) { // single args
        args[0] = input;
        args[1] = args[2] = NULL;
    } else { // multiple args
        *p = '\0';
        args[0] = input;
        input = ++p;
        p = strchr(input, ' ');
        if (p == NULL) { // 2 args
            args[1] = input;
            args[2] = NULL;
        } else { // 3 args
            *p = '\0';
            args[1] = input;
            args[2] = p + 1;
        }
    }
    return 0;
}

static int AddClient(struct in_addr ip, uint16_t port, int fd) {
    int target_index = -1;
    for (int i = 0; i < MAX_QUEUE; i++) {
        if (clients[i].id == 0) {
            target_index = i;
            break; 
        }
    }
    if (target_index == -1) {
        printf("[ERROR] Too Many Clients!\n");
        return -1;
    }

    clients[target_index].id = target_index + 1;
    clients[target_index].ip = ip;
    clients[target_index].port = port;
    clients[target_index].fd = fd;
    cur_client_num++;

    return target_index + 1;
}

static void CloseClient(int fd) {
    for (int i = 0; i < MAX_QUEUE; i++) {
        if (clients[i].fd == fd) {
            close(fd);
            clients[i].id = 0;
            cur_client_num--;
            break;
        }
    }
}

static int FindIdByFd(int fd) {
    for (int i = 0; i < MAX_QUEUE; i++) {
        if (clients[i].fd == fd) {
            return clients[i].id;
        }
    }
    return 0;
}

static int FindFdById(int id) {
    for (int i = 0; i < MAX_QUEUE; i++) {
        if (clients[i].id == id) {
            return clients[i].fd;
        }
    }
    return 0;
}

static int ExistId(int id) {
    for (int i = 0; i < MAX_QUEUE; i++) {
        if (clients[i].id == id) {
            return 1;
        }
    }
    return 0;
}

static void *Handler(void *accept_fd) {
    int client_fd = *(int*)accept_fd;
    char buf[BUF_SIZE];
    int recv_num = 0;
    int send_num = 0;
    char *args[ARG_NUM] = {0};
    int parse_res = 0;

    while (1) {
        memset(buf, 0, BUF_SIZE);
        recv_num = read(client_fd, buf, BUF_SIZE);
        if (recv_num < 0) {
            printf("[ERROR] Read Error!\n");
            break;
        } else if (recv_num == 0) {
            printf("[INFO] Read Nothing! Client May Close!\n");
            break;
        }
        parse_res = ParseInput(buf, args);
        if (parse_res == 1) {
            printf("[ERROR] Parse Error!\n");
            continue;
        }

        if (strcmp(args[0], "time") == 0) {
            time_t clock;
            char time_str[30];
            time(&clock);
            strcpy(time_str, ctime(&clock));
            sprintf(buf, "[HOST] %s", time_str);
            write(client_fd, buf, BUF_SIZE);
        } else if (strcmp(args[0], "name") == 0) {
            char hostname[30];
            gethostname(hostname, sizeof(hostname));
            sprintf(buf, "[HOST] %s\n", hostname);
            write(client_fd, buf, BUF_SIZE);
        } else if (strcmp(args[0], "list") == 0) {
            char *p = buf;
            sprintf(p, "[HOST] Client Num: %d\n", cur_client_num);
            p += strlen(p);
            for (int i = 0; i < MAX_QUEUE; i++) {
                if (clients[i].id != 0) {
                    sprintf(p, "\t[ID] %02d [IP] %s [PORT] %d\n", clients[i].id, inet_ntoa(clients[i].ip), clients[i].port);
                    p += strlen(p);
                }
            }
            write(client_fd, buf, BUF_SIZE);
        } else if (strcmp(args[0], "send") == 0) {
            if (args[1] == NULL) {
                printf("[ERROR] Lack Args!\n");
                continue;
            }
            int cur_id = FindIdByFd(client_fd);
            int target_id = atoi(args[1]);
            if (target_id == 0 || target_id == cur_id || !ExistId(target_id)) {
                sprintf(buf, "[HOST] Invalid Id!\n");
                write(client_fd, buf, BUF_SIZE);
                continue;
            } else {
                sprintf(buf, "[HOST] Communication Established!\n");
                write(client_fd, buf, BUF_SIZE);
            }

            int target_fd = FindFdById(target_id);
            char buf2[BUF_SIZE];
            while (1) {
                recv_num = read(client_fd, buf, BUF_SIZE);
                if (recv_num < 0) {
                    printf("[ERROR] Read Error!\n");
                    break;
                } else if (recv_num == 0) {
                    printf("[INFO] Read Nothing! Client May Close!\n");
                    break;
                }
                if (strcmp(buf, "\n") == 0) {
                    break;
                } else {
                    sprintf(buf2, "[CLIENT %02d] %s", cur_id, buf);
                    write(target_fd, buf2, BUF_SIZE);
                }
            }
        } else {
            printf("[ERROR] Invalid Instruction!\n");
        }
    }

    CloseClient(client_fd);
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("[ERROR] Lack Port! Usage: ./server [PORT]\n");
        exit(1);
    }
    int port;
    int socket_fd, accept_fd;
    struct sockaddr_in serv_addr, client_addr;
    int client_size = sizeof(client_addr);

    if ((port = atoi(argv[1])) == 0) {
        printf("[ERROR] Invalid Port!\n");
        exit(1);
    }
    
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("[ERROR] Socket Error!\n");
        exit(1);
    } else {
        printf("[OK] Socket Success! Socket: %d\n", socket_fd);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ((bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr))) == -1) {
        printf("[ERROR] Bind Error!\n");
        exit(1);
    } else {
        printf("[OK] Bind Succcess! IP: %s, Port: %d\n", inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
    }

    if ((listen(socket_fd, MAX_QUEUE)) == -1) {
        printf("[ERROR] Listen Error!\n");
        exit(1);
    } else {
        printf("[OK] Listen Success! Waiting for Connection!\n");
    }

    while (1) {
        pthread_t thread_id;

        if ((accept_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_size)) == -1) {
            printf("[ERROR] Accept Error!\n");
            continue;
        } else {
            struct in_addr ip = client_addr.sin_addr;
            uint16_t port = ntohs(client_addr.sin_port);
            int id = AddClient(ip, port, accept_fd);
            printf("[OK] Client %d Connect! IP: %s, Port: %d\n", id, inet_ntoa(ip), port);
        }

        if(pthread_create(&thread_id, NULL, Handler, (void *)(&accept_fd)) == -1)
        {
            printf("[ERROR] Pthread Create Error!\n");
            continue;;
        }
    }
}