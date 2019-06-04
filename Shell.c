/**
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.
            
            *---*---------------------------------*---*
            *---*   TO RUN READ FIRST README.md   *---*
            *---*               OR                *---*
            *---*        chmod +x run_shell       *---*
            *---*           ./run_shell           *---*
            *---*---------------------------------*---*

**/

#include <string.h>

#include "common/common.h"
#include "job/job_control.h"   // remember to compile with module job_control.c
#include "history/history.h"
#include "pipe/pipe.h"
#include "internal_commands/internal_commands.h"

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------
job *global_jobs;
command_used *historial_commands;

void handler_SIGCHLD() {
    int status;
    int info;
    job *tarea;

    // block_SIGCHLD();
    for (int i = 1; i <= list_size(global_jobs); ++i) {
        tarea = get_item_bypos(global_jobs, i);
        int pid_wait = waitpid(tarea->pgid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (pid_wait > 0) {

            int status_res = analyze_status(status, &info);

            if (status_res == EXITED || (status_res == SIGNALED && (info == SIGKILL || info == SIGTERM))) {
                print_info(tarea->state, tarea->pgid, tarea->command, status_res, info);
                delete_job(global_jobs, tarea);
                i--;
            } else if (status_res == SUSPENDED || (status_res == SIGNALED && info == SIGSTOP)) {
                tarea->state = STOPPED;
                print_info(tarea->state, tarea->pgid, tarea->command, status_res, info);
            } else if (status_res == CONTINUED) {
                tarea->state = BACKGROUND;
                print_info(tarea->state, tarea->pgid, tarea->command, status_res, info);
            }
        }
    }
    // unblock_SIGCHLD();
}

void run_parent(pid_t pid_fork[], char *args[], int background) {
    int status;             /* status returned by wait */
    enum status status_res; /* status processed by analyze_status() */
    int info;                /* info processed by analyze_status() */

    new_process_group(pid_fork[0]);

    if (args[1] != NULL && !strcpy(args[1], "|")) {

        if (!background) {

            /* FUNCIONAMIENTO PRINCIPAL */
            set_terminal(pid_fork[0]);  // le da el terminal al hijo
            for (int i = 0; i < maxChilds; i++) { 
                waitpid(pid_fork[i], &status, WUNTRACED);   // espera por el hijo, pid_wait devuelve el pid del hijo
            }
            set_terminal(getpid());                             // le da el terminal al padre
            /* ------------------------ */

            status_res = analyze_status(status, &info);

            if (status_res == SUSPENDED) {
                block_SIGCHLD();
                add_job(global_jobs, new_job(pid_fork[0], args[0], STOPPED));
                unblock_SIGCHLD();
            } else if (info == 0xFF) {
                fprintf(stderr, "Error, command not found: %s\n", args[0]);
                fflush(stderr);
            }

            print_info(background, pid_fork[0], args[0], status_res, info);

        } else {
            printf("%s job running... pid: %i, command: %s\n", state_strings[background], pid_fork[0], args[0]);
            block_SIGCHLD();
            add_job(global_jobs, new_job(pid_fork[0], args[0], background));
            unblock_SIGCHLD();
        }

    } else {

        if (!background) {

            /* FUNCIONAMIENTO PRINCIPAL */
            set_terminal(pid_fork[0]);  // le da el terminal al hijo
            // for (int i = 0; i < maxChilds; i++) { 
                waitpid(pid_fork[0], &status, WUNTRACED);   // espera por el hijo, pid_wait devuelve el pid del hijo
            // }
            set_terminal(getpid());                             // le da el terminal al padre
            /* ------------------------ */

            status_res = analyze_status(status, &info);

            if (status_res == SUSPENDED) {
                block_SIGCHLD();
                add_job(global_jobs, new_job(pid_fork[0], args[0], STOPPED));
                unblock_SIGCHLD();
            } else if (info == 0xFF) {
                fprintf(stderr, "Error, command not found: %s\n", args[0]);
                fflush(stderr);
            }

            print_info(background, pid_fork[0], args[0], status_res, info);

        } else {
            printf("%s job running... pid: %i, command: %s\n", state_strings[background], pid_fork[0], args[0]);
            block_SIGCHLD();
            add_job(global_jobs, new_job(pid_fork[0], args[0], background));
            unblock_SIGCHLD();
        }

    }
}

void run_child(char *args[], int pid_fork[], int descf[], int * fno) {
    restore_terminal_signals();

    if (args[1] != NULL && !strcpy(args[1], "|")) {
        if (pid_fork[0] == getpid())
        {
            /* el proceso padre ejecuta el primer programa y cambia su
            salida estandar al pipe cerrando la entrada del pipe */
            *fno=fileno(stdout);
            dup2(descf[1], *fno);
            close(descf[0]);
            execlp(args[0], args[0], NULL);
        }
        else if (pid_fork[1] == getpid())
        {
            /* el proceso hijo tiene una copia del pipe del padre,
            en el fork, ejecuta el segundo programa y cambia su
            entrada estandar por el pipe cerrando la salida del pipe */
            *fno=fileno(stdin);
            dup2(descf[0], *fno);
            close(descf[1]);
            execlp(args[2], args[2], NULL);
        }
        // exec_pipe(args);
    } else {
        execvp(args[0], args);
        exit(-1);
    }   
}

int main(void) {
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE / 2];     /* command line (of 256) has max of 128 arguments */
    pid_t pid_fork[maxChilds];

    global_jobs = new_list("jobs");
    historial_commands = new_historial_list();

    signal(SIGCHLD, handler_SIGCHLD);

    ignore_terminal_signals();

    while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
    {
        fflush(stdout);
        printf(VERDE"COMMAND->"$);
        fflush(stdout);

        get_command_propio(inputBuffer, MAX_LINE, args, &background, historial_commands);
        
        if (args[0] == NULL) continue;   // if empty command
        
        if (strcmp(args[0], "historial") && !(args[1] != NULL && !strcmp(args[1], "|"))) {
            add_command_used(historial_commands, new_command(args[0], args, background));
        }

        int has_runned_internal_command = run_interal_commands(global_jobs, historial_commands, args, &background);
        if (has_runned_internal_command) {
            continue;
        }


        int descf[2], fno;
        if (args[1] != NULL && !strcpy(args[1], "|")) {
            printf("HOLA SOY UNA PIPE");
            pipe(descf);

            for (int i = 0; i < maxChilds; i++) {
                pid_fork[i] = fork();
            }

            for (int i = 0; i < maxChilds && pid_fork[i] == 0; i++) {
                run_child(args, pid_fork, descf, &fno);
            }
            // int isChild1 = pid_fork[0] == 0;
            // int isChild2 = pid_fork[1] == 0;
            // if (isChild1) {
            //     run_child(args, pid_fork, descf, &fno);
            // } else if (isChild2) {
            //     run_child(args, pid_fork, descf, &fno);
            if (pid_fork[0] && pid_fork[1]) { // Parent
                run_parent(pid_fork, args, background);
            }
        } else {
            
            pid_fork[0] = fork();
            int isChild = pid_fork[0] == 0;
            if (isChild) {
                run_child(args, pid_fork, descf, &fno);
            } else { // Parent
                run_parent(pid_fork, args, background);
            }
        }
        
    } // end while
}

// Tarea 3:

// SIGCHILD -> SIGNAL -> MANAGER
// LISTA -> pidw = waitpid(pid, &status, WNOHANG | WUNTRACED )
// pidw == pid -> mirar status (signaled, stopped)
// si ocurre algo de lo que le preguntas en el tercer parametro
// el waitpid te devuelve el pid del proceso al que le ha ocurrido algo

// waitpid(pid, &status,        WNOHANG | WUNTRACED | WCONTINUED)
// pidw == pid -> mirar status (signaled, stopped, continued)
// borrar de la lista el proceso si signaled o stopped

// introducir en la lista de jobs cuando hay un proceso en segundo plano o un proceso en primer plano suspendido