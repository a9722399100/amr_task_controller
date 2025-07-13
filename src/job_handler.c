#include "../include/job_handler.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <time.h>

void send_log(int client_sock, const char* msg, int client_id) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "[Client%d] %s", client_id, msg);
    printf("[ServerLog] %s\n", buffer);
    fflush(stdout);
    send(client_sock, buffer, strlen(buffer), 0);
    send(client_sock, "\n", 1, 0);
}

void print_task_summary(Task* task) {
    char time_msg[256];
    snprintf(time_msg, sizeof(time_msg),
             "[Summary] 任務ID=%d 等待=%lds 執行=%lds 總共=%lds",
             task->task_id,
             task->start_time - task->wait_start_time,
             task->end_time - task->start_time,
             task->end_time - task->wait_start_time);
    send_log(task->client_sock, time_msg, task->client_id);
}

void handle_material_delivery(Task* task, const char* from, const char* to) {
    send_log(task->client_sock, "[Cargo] 等待可用資源...", task->client_id);
    sem_wait(&cargo_sem);
    task->status = TASK_RUNNING;
    task->start_time = time(NULL);

    send_log(task->client_sock, "[Cargo] 已取得資源，開始執行物料配送...", task->client_id);
    send_log(task->client_sock, "[Cargo] 裝載物料中...", task->client_id);
    sleep(10);
    send_log(task->client_sock, "[Cargo] 運送中...", task->client_id);
    sleep(10);
    send_log(task->client_sock, "[Cargo] 卸料完成！", task->client_id);

    task->status = TASK_COMPLETED;
    task->end_time = time(NULL);
    sem_post(&cargo_sem);

    print_task_summary(task);
}

void handle_intermediate_transfer(Task* task, const char* from, const char* to) {
    send_log(task->client_sock, "[Transfer] 等待可用資源...", task->client_id);
    sem_wait(&cargo_sem);
    task->status = TASK_RUNNING;
    task->start_time = time(NULL);

    send_log(task->client_sock, "[Transfer] 已取得資源，開始搬運半成品...", task->client_id);
    send_log(task->client_sock, "[Transfer] 開始搬運半成品...", task->client_id);
    sleep(10);
    send_log(task->client_sock, "[Transfer] 運送中...", task->client_id);
    sleep(10);
    send_log(task->client_sock, "[Transfer] 已完成中段轉移！", task->client_id);

    task->status = TASK_COMPLETED;
    task->end_time = time(NULL);
    sem_post(&cargo_sem);

    print_task_summary(task);
}

void handle_sample_collection(Task* task, const char* source) {
    send_log(task->client_sock, "[QC] 等待可用資源...", task->client_id);
    sem_wait(&qc_sem);
    task->status = TASK_RUNNING;
    task->start_time = time(NULL);

    send_log(task->client_sock, "[QC] 已取得資源，開始樣本採集任務...", task->client_id);
    send_log(task->client_sock, "[QC] 移動至樣本採集中心中...", task->client_id);
    sleep(10);
    send_log(task->client_sock, "[QC] 進行樣本採集...", task->client_id);
    sleep(10);
    send_log(task->client_sock, "[QC] 已完成樣本採集任務！", task->client_id);

    task->status = TASK_COMPLETED;
    task->end_time = time(NULL);
    sem_post(&qc_sem);

    print_task_summary(task);
}

void handle_job(Task* task) {
    char job[64], from[64], to[64], source[64];
    memset(job, 0, sizeof(job));
    memset(from, 0, sizeof(from));
    memset(to, 0, sizeof(to));
    memset(source, 0, sizeof(source));

    sscanf(task->job_desc, "REQUEST job=%[^;]; from=%[^;]; to=%s", job, from, to);
    if (strlen(job) == 0) {
        sscanf(task->job_desc, "REQUEST job=%[^;]; source=%s", job, source);
    }

    if (strcmp(job, "material_delivery") == 0) {
        handle_material_delivery(task, from, to);
    } else if (strcmp(job, "intermediate_transfer") == 0) {
        handle_intermediate_transfer(task, from, to);
    } else if (strcmp(job, "sample_collection") == 0) {
        handle_sample_collection(task, source);
    } else {
        send_log(task->client_sock, "[Error] 無效的任務參數！", task->client_id);
        task->status = TASK_CANCELLED;
    }
}

