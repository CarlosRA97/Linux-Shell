/**
UNIX Shell Project

Sistemas Operativos
Grados I. Informatica, Computadores & Software
Dept. Arquitectura de Computadores - UMA

Some code adapted from "Fundamentos de Sistemas Operativos", Silberschatz et al.

To compile and run the program:
   $ gcc Shell_project.c job_control.c -o Shell
   $ ./Shell          
	(then type ^D to exit program)

**/

#define ROJO "\x1b[31;1;1m"
#define NEGRO "\x1b[0m"
#define VERDE "\x1b[32;1;1m"
#define AZUL "\x1b[34;1;1m"
#define CIAN "\x1b[36;1;1m"
#define MARRON "\x1b[33;1;1m"
#define PURPURA "\x1b[35;1;1m"
#define $ "\e[0m"

#include "job_control.h"   // remember to compile with module job_control.c
#include <string.h>
#include "history.h"

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------
job *global_jobs;

void print_info(int state, pid_t pid, char *command, enum status status_res, int info) {
    printf("%s pid: %i, command: %s, %s, info: %i\n",
           state_strings[state],
           pid,
           command,
           status_strings[status_res],
           info);
}

void handler_SIGCHLD() {
    int status;
    int info;
    job *tarea;

    block_SIGCHLD();
    for (int i = 1; i <= list_size(global_jobs); ++i) {
        tarea = get_item_bypos(global_jobs, i);
        int pid_wait = waitpid(tarea->pgid, &status, WNOHANG | WUNTRACED | WCONTINUED);

        if (pid_wait > 0) {

            int status_res = analyze_status(status, &info);

            if (status_res == EXITED || (status_res == SIGNALED && (info == SIGKILL || info == SIGTERM))) {
                delete_job(global_jobs, tarea);
                i--;
            } else if (status_res == SUSPENDED || (status_res == SIGNALED && info == SIGSTOP)) {
                tarea->state = STOPPED;
            } else if (status_res == CONTINUED) {
                tarea->state = BACKGROUND;
            }

            print_info(tarea->state, tarea->pgid, tarea->command, status_res, info);
        }
    }
    unblock_SIGCHLD();
}

int get_pos(char *arg) {
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

void fg(char *arg) {
    block_SIGCHLD();
    int pos = get_pos(arg);
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
        } else {
            print_info(tarea->state, tarea->pgid, tarea->command, status_res, info);
        }

    }
    unblock_SIGCHLD();
}

void bg(char *arg) {
    block_SIGCHLD();
    int pos = get_pos(arg);
    if (pos) {

        job *tarea = get_item_bypos(global_jobs, pos);

        tarea->state = BACKGROUND;
        killpg(tarea->pgid, SIGCONT);

    }
    unblock_SIGCHLD();
}

void cd(char *arg) {
    int exitCode = chdir(arg);
    if (arg != NULL && exitCode == -1) {
        fprintf(stderr, "No se puede cambiar al directorio %s\n", arg);
        fflush(stdout);
    }
}

void run_parent(pid_t pid_fork, char *args[], int background) {
    int pid_wait; /* pid for created and waited process */
    int status;             /* status returned by wait */
    enum status status_res; /* status processed by analyze_status() */
    int info;                /* info processed by analyze_status() */

    new_process_group(pid_fork);

    if (!background) {

        /* FUNCIONAMIENTO PRINCIPAL */
        set_terminal(pid_fork);                             // le da el terminal al hijo
        pid_wait = waitpid(pid_fork, &status, WUNTRACED);   // espera por el hijo, pid_wait devuelve el pid del hijo
        set_terminal(getpid());                             // le da el terminal al padre
        /* ------------------------ */

        status_res = analyze_status(status, &info);

        if (status_res == SUSPENDED) {
            block_SIGCHLD();
            add_job(global_jobs, new_job(pid_fork, args[0], STOPPED));
            unblock_SIGCHLD();
        } else if (info == 0xFF) {
            fprintf(stderr, "Error, command not found: %s\n", args[0]);
            fflush(stderr);
        }

        print_info(background, pid_fork, args[0], status_res, info);

    } else {
        printf("%s job running... pid: %i, command: %s\n", state_strings[background], pid_fork, args[0]);
        block_SIGCHLD();
        add_job(global_jobs, new_job(pid_fork, args[0], background));
        unblock_SIGCHLD();
    }
}

void run_child(char *args[]) {
    restore_terminal_signals();
    execvp(args[0], args);
    exit(-1);
}

int run_interal_commands(char *args[]) {
    int has_runned = 0;
    if (!strcmp(args[0], "cd")) {
        cd(args[1]);
        has_runned = 1;
    } else if (!strcmp(args[0], "exit")) {
        exit(0);
    } else if (!strcmp(args[0], "jobs")) {
        block_SIGCHLD();
        if (!empty_list(global_jobs)) {
            print_job_list(global_jobs);
        }
        unblock_SIGCHLD();
        has_runned = 1;
    } else if (!strcmp(args[0], "fg")) {
        fg(args[1]);
        has_runned = 1;
    } else if (!strcmp(args[0], "bg")) {
        bg(args[1]);
        has_runned = 1;
    }
    return has_runned;
}

void read_arrow_keys() {
    /* Las teclas de cursor devuelven una secuencia de 3 caracteres,
    27 -­­ 91 -­­ (65, 66, 67 ó 68) */

    const char ESC = 0x1B;
    const char LEFT_SQUARE_BRACKET = 0x5B;

    char sec[3];
    sec[0] = getch();
    switch (sec[0])
    {
        case ESC:
            sec[1] = getch();
            if (sec[1] == LEFT_SQUARE_BRACKET) // 27,91,...
            {
                sec[2] = getch();
                switch (sec[2])
                {
                    case 65: /* ARRIBA */
                        // printf("Arriba");
                    break;
                    case 66: /* ABAJO */
                        // printf("Abajo");
                    break;
                    case 67: /* DERECHA */
                        // printf("Derecha");
                    break;
                    case 68: /* IZQUIERDA */
                        // printf("Izquierda");
                    break;
                    default:
                    break;
                }
            }
        break;
        case 0x7B: /* BORRAR */
            break;
        default:
            break;
    }

}

int main(void) {
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE / 2];     /* command line (of 256) has max of 128 arguments */

    global_jobs = new_list("jobs");

    signal(SIGCHLD, handler_SIGCHLD);

    ignore_terminal_signals();


    while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
    {
        printf(VERDE"COMMAND->"$);
        fflush(stdout);
        // read_arrow_keys();
        fflush(stdout);
        get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */

        if (args[0] == NULL) continue;   // if empty command

        int has_runned_internal_command = run_interal_commands(args);
        if (has_runned_internal_command) {
            continue;
        }

        int pid_fork = fork();

        int isChild = pid_fork == 0;
        if (isChild) {
            run_child(args);
        } else { // Child
            run_parent(pid_fork, args, background);
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