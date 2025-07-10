#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <signal.h>

#include "../include/job_handler.h"
#include "../include/task_queue.h"

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

sem_t cargo_sem;
sem_t qc_sem;

TaskQueue cargo_queue;  // 使用 struct 而非指標

int server_fd;

// Cargo 任務專屬 dispatcher thread
void* dispatcher_thread(void* arg) {
    while (1) {
        Task* task = dequeue_task(&cargo_queue);
        if (task) {
            handle_job(task->client_sock, task->job_desc, task->client_id, task->priority);
            close(task->client_sock);
            free(task);  // 任務記憶體釋放
        }
    }
    return NULL;
}

// 每個 client 對應的處理程序
void* client_handler(void* arg) {
    int* data = (int*)arg;
    int client_sock = data[0];
    int client_id = data[1];
    free(data);

    char buffer[BUFFER_SIZE];
    int len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (len > 0) {
        buffer[len] = '\0';

        // 解析 priority
        int priority = 5;  // 預設最低優先級
        char* prio_ptr = strstr(buffer, "priority=");
        if (prio_ptr) {
            sscanf(prio_ptr, "priority=%d", &priority);
        }

        // 根據任務類型決定：排入 cargo queue 或立即處理
        if (strstr(buffer, "material_delivery") || strstr(buffer, "intermediate_transfer")) {
            enqueue_task(&cargo_queue, client_sock, client_id, buffer, priority);
        } else {
            handle_job(client_sock, buffer, client_id, priority);
            close(client_sock);
        }
    } else {
        close(client_sock);
    }

    return NULL;
}

// Ctrl+C 時的釋放邏輯
void cleanup(int signum) {
    close(server_fd);
    sem_destroy(&cargo_sem);
    sem_destroy(&qc_sem);
    destroy_task_queue(&cargo_queue);
    printf("\n[Server] 資源已釋放，關閉服務\n");
    exit(0);
}

int main() {
    signal(SIGINT, cleanup);

    sem_init(&cargo_sem, 0, 3);
    sem_init(&qc_sem, 0, 1);
    init_task_queue(&cargo_queue);  // 初始化 queue

    // 啟動 cargo dispatcher
    pthread_t dispatcher;
    pthread_create(&dispatcher, NULL, dispatcher_thread, NULL);
    pthread_detach(dispatcher);

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
        pthread_detach(tid);
    }

    return 0;
}


