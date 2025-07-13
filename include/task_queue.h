#ifndef TASK_QUEUE_H
#define TASK_QUEUE_H

#include <pthread.h>
#include <time.h>

typedef enum {
    TASK_PENDING,
    TASK_RUNNING,
    TASK_COMPLETED,
    TASK_CANCELLED
} TaskStatus;

typedef struct Task {
    int task_id;
    int client_sock;
    int client_id;
    int priority;
    char job_desc[256];
    TaskStatus status;
    time_t created_at;
    time_t wait_start_time;
    time_t start_time;
    time_t end_time;
    struct Task* next;
} Task;

typedef struct {
    Task* head;
    int next_task_id;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} TaskQueue;

void init_task_queue(TaskQueue* q);
void enqueue_task(TaskQueue* q, Task* new_task);
Task* dequeue_task(TaskQueue* q);
void destroy_task_queue(TaskQueue* q);

#endif
