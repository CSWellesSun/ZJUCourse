#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#define BUF_SIZE 2048
#define ARG_NUM 3

pthread_cond_t cond = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread_id;

int socket_fd = -1;
char buf[BUF_SIZE];

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

static int Connect(char *hostname, int port) {
    int socket_fd = -1;
    unsigned long int host;
    struct sockaddr_in serv_addr;
    
    if ((host = inet_addr(hostname)) == -1) {
        printf("[ERROR] Invalid Host!\n");
        return -1;
    }

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("[ERROR] Socket Error!\n");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = host;

    if ((connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr))) == -1) {
        return -1;    
    }

    return socket_fd;
}

static int IsFromClient(char *buf) {
    char buf2[BUF_SIZE];
    strncpy(buf2, buf, BUF_SIZE);
    char *p = strchr(buf2, ' ');
    if (p != NULL) {
        *p = '\0';
        if (strcmp(buf2, "[HOST]") != 0) { // client
            return 1;
        }
    }
    return 0;
}

void *Listen(void *arg) {
    int recv_num;
    char buf_listen[BUF_SIZE];

    while (1) {
        recv_num = read(socket_fd, buf_listen, BUF_SIZE);
        if (IsFromClient(buf_listen)) {
            printf("%s", buf_listen);
        } else { // server
            pthread_mutex_lock(&mtx);
            strncpy(buf, buf_listen, BUF_SIZE);
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mtx);
        }
    }
    return NULL;
}

static int Recv() {
    pthread_mutex_lock(&mtx);
    pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
}

int main() {
    printf("-----------------------------------------Client-----------------------------------------\n");
    printf("Usage: \n");
    printf("1. connect [IP] [PORT]: connect to server\n");
    printf("2. exit: shutdown the connection\n");
    printf("3. time: show server time\n");
    printf("4. name: show client name\n");
    printf("5. list: show the clients connected to the server\n");
    printf("6. send [ID]: start to send [MSG] to client [ID], press Enter to stop sending message\n");
    printf("----------------------------------------------------------------------------------------\n");

    char *args[ARG_NUM] = {0};
    int parse_res;

    while (1) {
        printf("> ");
        fgets(buf, BUF_SIZE, stdin);
        parse_res = ParseInput(buf, args);
        if (parse_res == 1) {
            printf("[ERROR] Parse Error!\n");
            continue;
        }

        if (strcmp(args[0], "connect") == 0) {
            if (socket_fd != -1) {
                printf("[ERROR] Already Connected!\n");
                continue;
            }
            if (args[1] == NULL || args[2] == NULL) {
                printf("[ERROR] Lack Args!\n");
                continue;
            }
            int port = atoi(args[2]);
            if (port == 0) {
                printf("[ERROR] Invalid Port!\n");
                continue;
            }
            socket_fd = Connect(args[1], port);
            if (socket_fd == -1) {
                printf("[ERROR] Connect Fail\n");
            } else {
                printf("[OK] Connect Success!\n");
            }
            // start a thread to listen
            pthread_create(&thread_id, NULL, Listen, NULL);
        } else if (strcmp(args[0], "exit") == 0) {
            if (socket_fd != -1) {
                close(socket_fd);
                pthread_cancel(thread_id);
            }
            pthread_mutex_destroy(&mtx);
            pthread_cond_destroy(&cond);
            break;
        } else if (strcmp(args[0], "time") == 0) {
            if (socket_fd == -1) {
                printf("[ERROR] Unconnected!\n");
                continue;
            }
            sprintf(buf, "time");
            write(socket_fd, buf, 5);
            Recv();
            printf("%s", buf);
        } else if (strcmp(args[0], "name") == 0) {
            if (socket_fd == -1) {
                printf("[ERROR] Unconnected!\n");
                continue;
            }
            sprintf(buf, "name");
            write(socket_fd, buf, 5);
            Recv();
            printf("%s", buf);
        } else if (strcmp(args[0], "list") == 0) {
            if (socket_fd == -1) {
                printf("[ERROR] Unconnected!\n");
                continue;
            }
            sprintf(buf, "list");
            write(socket_fd, buf, 5);
            Recv();
            printf("%s", buf);
        } else if (strcmp(args[0], "send") == 0) {
            if (args[1] == NULL) {
                printf("[ERROR] Lack Args!\n");
                continue;
            }
            // send instruction
            sprintf(buf, "%s %s", args[0], args[1]);
            write(socket_fd, buf, BUF_SIZE);
            // check validity of communition
            Recv();
            if (strcmp(buf, "[HOST] Invalid Id!\n") == 0) {
                printf("%s", buf);
                continue;
            }

            while (1) {
                printf(">> ");
                fgets(buf, BUF_SIZE, stdin);
                write(socket_fd, buf, BUF_SIZE);
                if (strcmp(buf, "\n") == 0) break;
            }

        } else {
            printf("[ERROR] Invalid Instruction!\n");
        }
    }
}