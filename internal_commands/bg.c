#include "internal_commands.h"

void bg(job * global_jobs, char *arg) {
    block_SIGCHLD();
    int pos = get_pos(global_jobs, arg);
    if (pos) {

        job *tarea = get_item_bypos(global_jobs, pos);

        tarea->state = BACKGROUND;
        killpg(tarea->pgid, SIGCONT);

    }
    unblock_SIGCHLD();
}