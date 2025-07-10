#ifndef JOB_HANDLER_H
#define JOB_HANDLER_H

#include <semaphore.h>

extern sem_t cargo_sem;
extern sem_t qc_sem;

void handle_job(int client_sock, const char* job_desc, int client_id, int priority);
void send_log(int client_sock, const char* msg, int client_id);

#endif

