#include "../include/task_queue.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

void init_task_queue(TaskQueue* q) {
    q->head = NULL;
    q->next_task_id = 1;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void enqueue_task(TaskQueue* q, Task* new_task) {
    pthread_mutex_lock(&q->mutex);

    new_task->wait_start_time = time(NULL);
    new_task->start_time = 0;
    new_task->end_time = 0;
    new_task->next = NULL;
    new_task->status = TASK_PENDING;

    // 插入排序：priority 越小越優先
    if (q->head == NULL || new_task->priority < q->head->priority) {
        new_task->next = q->head;
        q->head = new_task;
    } else {
        Task* current = q->head;
        while (current->next != NULL && current->next->priority <= new_task->priority) {
            current = current->next;
        }
        new_task->next = current->next;
        current->next = new_task;
    }

    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

Task* dequeue_task(TaskQueue* q) {
    pthread_mutex_lock(&q->mutex);
    while (q->head == NULL) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    Task* task = q->head;
    q->head = q->head->next;
    task->next = NULL;

    task->status = TASK_RUNNING;
    task->start_time = time(NULL);  // 寫入執行開始時間

    pthread_mutex_unlock(&q->mutex);
    return task;
}

void destroy_task_queue(TaskQueue* q) {
    pthread_mutex_lock(&q->mutex);
    Task* current = q->head;
    while (current != NULL) {
        Task* temp = current;
        current = current->next;
        free(temp);
    }
    pthread_mutex_unlock(&q->mutex);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}

