#include "../include/job_handler.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/socket.h>

void send_log(int client_sock, const char* msg, int client_id) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), "[Client%d] %s", client_id, msg);
    printf("[ServerLog] %s\n", buffer);
    fflush(stdout);
    send(client_sock, buffer, strlen(buffer), 0);
    send(client_sock, "\n", 1, 0);
}

void handle_material_delivery(int client_sock, const char* from, const char* to, int client_id) {
    send_log(client_sock, "[Cargo] 等待可用資源...", client_id);
    sem_wait(&cargo_sem);
    send_log(client_sock, "[Cargo] 已取得資源，開始執行物料配送...", client_id);
    send_log(client_sock, "[Cargo] 裝載物料中...", client_id);
    sleep(2);
    send_log(client_sock, "[Cargo] 運送中...", client_id);
    sleep(2);
    send_log(client_sock, "[Cargo] 卸料完成！", client_id);
    sem_post(&cargo_sem);
}

void handle_intermediate_transfer(int client_sock, const char* from, const char* to, int client_id) {
    send_log(client_sock, "[Transfer] 等待可用資源...", client_id);
    sem_wait(&cargo_sem);
    send_log(client_sock, "[Transfer] 已取得資源，開始搬運半成品...", client_id);
    send_log(client_sock, "[Transfer] 開始搬運半成品...", client_id);
    sleep(2);
    send_log(client_sock, "[Transfer] 運送中...", client_id);
    sleep(2);
    send_log(client_sock, "[Transfer] 已完成中段轉移！", client_id);
    sem_post(&cargo_sem);
}

void handle_sample_collection(int client_sock, const char* source, int client_id) {
    send_log(client_sock, "[QC] 等待可用資源...", client_id);
    sem_wait(&qc_sem);
    send_log(client_sock, "[QC] 已取得資源，開始樣本採集任務...", client_id);
    send_log(client_sock, "[QC] 移動至樣本採集中心中...", client_id);
    sleep(2);
    send_log(client_sock, "[QC] 進行樣本採集...", client_id);
    sleep(2);
    send_log(client_sock, "[QC] 已完成樣本採集任務！", client_id);
    sem_post(&qc_sem);
}

void handle_job(int client_sock, const char* job_desc, int client_id, int priority) {
    char job[64], from[64], to[64], source[64];
    memset(job, 0, sizeof(job));
    memset(from, 0, sizeof(from));
    memset(to, 0, sizeof(to));
    memset(source, 0, sizeof(source));

    sscanf(job_desc, "REQUEST job=%[^;]; from=%[^;]; to=%s", job, from, to);
    if (strlen(job) == 0) {
        sscanf(job_desc, "REQUEST job=%[^;]; source=%s", job, source);
    }

    if (strcmp(job, "material_delivery") == 0) {
        handle_material_delivery(client_sock, from, to, client_id);
    } else if (strcmp(job, "intermediate_transfer") == 0) {
        handle_intermediate_transfer(client_sock, from, to, client_id);
    } else if (strcmp(job, "sample_collection") == 0) {
        handle_sample_collection(client_sock, source, client_id);
    } else {
        send_log(client_sock, "[Error] 無效的任務參數！", client_id);
    }
}

