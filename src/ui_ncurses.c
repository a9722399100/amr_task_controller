#include "../include/task_queue.h"
#include <ncurses.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

extern TaskQueue cargo_queue;

void* ui_ncurses_thread(void* arg) {
    initscr();
    noecho();
    cbreak();
    curs_set(FALSE);

    while (1) {
        clear();
        mvprintw(0, 0, "AMR Task Monitor");
        mvprintw(1, 0, "------------------------");

        pthread_mutex_lock(&cargo_queue.mutex);
        Task* cur = cargo_queue.head;
        int row = 2;

        while (cur != NULL) {
            char status_str[16];
            switch (cur->status) {
                case TASK_PENDING: strcpy(status_str, "PENDING"); break;
                case TASK_RUNNING: strcpy(status_str, "RUNNING"); break;
                case TASK_COMPLETED: strcpy(status_str, "DONE"); break;
                case TASK_CANCELLED: strcpy(status_str, "CANCEL"); break;
                default: strcpy(status_str, "UNKNOWN"); break;
            }

            time_t now = time(NULL);
            int wait_s = (cur->status == TASK_PENDING) ? (int)(now - cur->wait_start_time) : 0;
            int run_s = (cur->status == TASK_RUNNING || cur->status == TASK_COMPLETED) ? (int)(cur->end_time - cur->start_time) : 0;

            mvprintw(row++, 0, "TaskID=%d  Client=%d", cur->task_id, cur->client_id);
            mvprintw(row++, 2, "Job=%s", cur->job_desc);
            mvprintw(row++, 2, "Status=%s  Wait=%ds  Run=%ds", status_str, wait_s, run_s);

            row++;
            cur = cur->next;
        }

        pthread_mutex_unlock(&cargo_queue.mutex);
        mvprintw(row + 1, 0, "[q] Quit Panel");
        refresh();

        timeout(1000); // refresh every 1 second
        int ch = getch();
        if (ch == 'q' || ch == 'Q') break;
    }

    endwin();
    return NULL;
}


