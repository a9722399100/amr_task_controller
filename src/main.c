#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <signal.h>
#include <time.h>

#include "../include/job_handler.h"
#include "../include/task_queue.h"
#include "../include/ui_ncurses.h"  // 使用 ncurses UI

#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 256

sem_t cargo_sem;
sem_t qc_sem;

TaskQueue cargo_queue;

int server_fd;

void* dispatcher_thread(void* arg) {
    while (1) {
        Task* task = dequeue_task(&cargo_queue);
        if (task) {
            handle_job(task);
            close(task->client_sock);
            free(task);
        }
    }
    return NULL;
}

void* client_handler(void* arg) {
    int* data = (int*)arg;
    int client_sock = data[0];
    int client_id = data[1];
    free(data);

    char buffer[BUFFER_SIZE];
    int len = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (len > 0) {
        buffer[len] = '\0';

        int priority = 5;
        char* prio_ptr = strstr(buffer, "priority=");
        if (prio_ptr) {
            sscanf(prio_ptr, "priority=%d", &priority);
        }

        Task* task = (Task*)malloc(sizeof(Task));
        task->client_sock = client_sock;
        task->client_id = client_id;
        task->priority = priority;
        task->status = TASK_PENDING;
        task->task_id = client_id;  // 暫時設定，進入 queue 後會被分配正式 ID
        task->created_at = time(NULL);
        task->wait_start_time = time(NULL);
        strncpy(task->job_desc, buffer, sizeof(task->job_desc));
        task->next = NULL;

        if (strstr(buffer, "material_delivery") || strstr(buffer, "intermediate_transfer")) {
            enqueue_task(&cargo_queue, task);
        } else {
            handle_job(task);
            close(client_sock);
            free(task);
        }
    } else {
        close(client_sock);
    }

    return NULL;
}

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
    init_task_queue(&cargo_queue);

    // 啟動 dispatcher thread
    pthread_t dispatcher;
    pthread_create(&dispatcher, NULL, dispatcher_thread, NULL);
    pthread_detach(dispatcher);

    // 啟動 ncurses UI thread（注意這裡已修正）
    pthread_t ui_thread;
    pthread_create(&ui_thread, NULL, ui_ncurses_thread, NULL);
    pthread_detach(ui_thread);

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

