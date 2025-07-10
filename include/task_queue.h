#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <pthread.h>

typedef struct Task {
    int client_sock;
    int client_id;
    int priority;
    char job_desc[256];
    struct Task* next;
} Task;

typedef struct {
    Task* head;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} TaskQueue;

void init_task_queue(TaskQueue* q);
void enqueue_task(TaskQueue* q, int client_sock, int client_id, const char* job_desc, int priority);
Task* dequeue_task(TaskQueue* q);
void destroy_task_queue(TaskQueue* q);

#endif


