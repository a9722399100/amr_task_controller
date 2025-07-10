#include "../include/task_queue.h"
#include <stdlib.h>
#include <string.h>

void init_task_queue(TaskQueue* q) {
    q->head = NULL;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

void enqueue_task(TaskQueue* q, int client_sock, int client_id, const char* job_desc, int priority) {
    Task* new_task = (Task*)malloc(sizeof(Task));
    new_task->client_sock = client_sock;
    new_task->client_id = client_id;
    new_task->priority = priority;
    strncpy(new_task->job_desc, job_desc, sizeof(new_task->job_desc));
    new_task->next = NULL;

    pthread_mutex_lock(&q->mutex);

    // 插入排序：priority 越小越優先
    if (q->head == NULL || priority < q->head->priority) {
        new_task->next = q->head;
        q->head = new_task;
    } else {
        Task* current = q->head;
        while (current->next != NULL && current->next->priority <= priority) {
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

