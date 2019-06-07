#include "internal_commands.h"

void fg(job * global_jobs, char *arg) {
    block_SIGCHLD();
    int isSignaled = 0;
    int pos = get_pos(global_jobs, arg);
    if (pos) {

        job *tarea_copy = get_item_bypos(global_jobs, pos);
        job *tarea = new_job(tarea_copy->pgid, tarea_copy->command, tarea_copy->state);
        delete_job(global_jobs, tarea_copy);

        set_terminal(tarea->pgid);

        if (tarea->state == STOPPED) {
            killpg(tarea->pgid, SIGCONT);
        }
        tarea->state = FOREGROUND;

        int status, info, status_res;

        int pid_wait = waitpid(tarea->pgid, &status, WUNTRACED);
        printf("pid_wait_sigchld: %i\n", pid_wait);
        set_terminal(getpid());

        status_res = analyze_status(status, &info);

        if (status_res == SUSPENDED) {
            tarea->state = STOPPED;
            printf("Suspended pid: %i, command %s\n", tarea->pgid, tarea->command);
            add_job(global_jobs, tarea);
        } else if (status_res == SIGNALED) {
            signaled++;
        } else {
            print_info(tarea->state, tarea->pgid, tarea->command, status_res, info);
        }

    }
    unblock_SIGCHLD();
}