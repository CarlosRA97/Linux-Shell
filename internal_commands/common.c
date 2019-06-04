#include "internal_commands.h"

int get_pos(job * global_jobs, char *arg) {
    int pos = 0;
    if (arg == NULL) {
        if (empty_list(global_jobs)) {
            fprintf(stderr, "No hay tareas\n");
        } else {
            pos = list_size(global_jobs);
        }
    } else if (atoi(arg) > list_size(global_jobs)) {
        fprintf(stderr, "Tarea no encontrada\n");
    } else {
        pos = atoi(arg);
    }
    return pos;
}

int run_interal_commands(job * global_jobs, command_used * historial_commands, char *args[], int * background) {
    char * command = args[0];
    int has_runned = 0;
    if (!strcmp(command, "cd")) {
        cd(args[1]);
        has_runned = 1;
    } else if (!strcmp(command, "exit")) {
        exit(0);
    } else if (!strcmp(command, "jobs")) {
        block_SIGCHLD();
        if (!empty_list(global_jobs)) {
            print_job_list(global_jobs);
        }
        unblock_SIGCHLD();
        has_runned = 1;
    } else if (!strcmp(command, "fg")) {
        fg(global_jobs, args[1]);
        has_runned = 1;
    } else if (!strcmp(command, "bg")) {
        bg(global_jobs, args[1]);
        has_runned = 1;
    } else if (!strcmp(command, "historial")) {
        has_runned = historial(historial_commands, args, background);
    }
    return has_runned;
}