#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include "../include/job_handler.h"
#include <signal.h>

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

sem_t cargo_sem;
sem_t qc_sem;

int server_fd;  // 提升為全域變數，供 cleanup 使用

// 客戶端處理程序
void* client_handler(void* arg) {
    int* data = (int*)arg;
    int client_sock = data[0];
    int client_id = data[1];
    free(data);

    char buffer[BUFFER_SIZE];
    int len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (len > 0) {
        buffer[len] = '\0';
        handle_job(client_sock, buffer, client_id);
    }

    close(client_sock);
    return NULL;
}

// 當 Ctrl+C 時清理 semaphore 與 socket
void cleanup(int signum) {
    close(server_fd); // 關閉 socket
    sem_destroy(&cargo_sem);
    sem_destroy(&qc_sem);
    printf("\n[Server] 資源已釋放，關閉服務\n");
    exit(0);
}

int main() {
    signal(SIGINT, cleanup);

    sem_init(&cargo_sem, 0, 3);  // 可同時處理 3 筆 cargo 任務
    sem_init(&qc_sem, 0, 1);     // 可同時處理 1 筆 QC 任務

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, MAX_CLIENTS);
    printf("Server listening on port %d...\n", PORT);

    int client_counter = 1;

    while (1) {
        int client_sock = accept(server_fd, NULL, NULL);
        int* pclient = malloc(sizeof(int) * 2);
        pclient[0] = client_sock;
        pclient[1] = client_counter++;

        pthread_t tid;
        pthread_create(&tid, NULL, client_handler, pclient);
        pthread_detach(tid);  // 自動釋放資源
    }

    return 0;
}

