#ifndef JOB_HANDLER_H
#define JOB_HANDLER_H

#include <semaphore.h>
#include "task_queue.h"

extern sem_t cargo_sem;
extern sem_t qc_sem;

void handle_job(Task* task);
void send_log(int client_sock, const char* msg, int client_id);

#endif

