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

#include "job_control.h"   // remember to compile with module job_control.c
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <sys/mman.h>
#include <assert.h>

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------
job * global_jobs;

typedef struct
{
    bool done;
    pthread_mutex_t mutex;
} shared_data;

static shared_data* data = NULL;

void initialise_shared()
{
    // place our shared data in shared memory
    int prot = PROT_READ | PROT_WRITE;
    int flags = MAP_SHARED | MAP_ANONYMOUS;
    data = mmap(NULL, sizeof(shared_data), prot, flags, -1, 0);
    assert(data);

    data->done = false;

    // initialise mutex so it works properly in shared memory
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(&data->mutex, &attr);
}

int waiting_pid;

void manejador_SIGCHILD() {
    int status;
    int info;
    printf("\n\nwaiting_pid: %i\n", waiting_pid);
    int pid_wait = waitpid(waiting_pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
    if (pid_wait == waiting_pid) {
        int status_res = analyze_status(status, &info);
        if (status_res == EXITED) {
            printf("El hijo termino correctamente");
            set_terminal(getpid());
        } else {
            printf("status res: %s, pid_wait: %i\n", status_strings[status_res], pid_wait);
        }
    }

//    if (!empty_list(jobs)) {
//        for (int i = 0; i < list_size(jobs); ++i) {
//            job *tarea = get_item_bypos(jobs, i);
//            printf("%s\n", state_strings[tarea->state]);
//            if (tarea->state == EXITED) {
//                delete_job(jobs, tarea);
//            }
//        }
//    }
}

int get_pos(char* arg) {
    return arg == NULL ? 1 : atoi(arg);
}

void fg(char * arg) {
    int pos = get_pos(arg);

    job *tarea = get_item_bypos(global_jobs, pos);
    if (tarea != NULL) {
        tarea->state = FOREGROUND;

        int status;
        int pid_child = tarea->pgid;
        int pid_wait = waitpid(pid_child, &status, WCONTINUED);
        if (pid_wait == pid_child) {
            set_terminal(pid_child);
        }
    }
}

void bg(char * arg) {
    int pos = get_pos(arg);

    job *tarea = get_item_bypos(global_jobs, pos);
    if (tarea != NULL) {
        tarea->state = BACKGROUND;
//
//        int status;
//        int pid_child = tarea->pgid;
//        int pid_wait = waitpid(pid_child, &status, WCONTINUED);
//        if (pid_wait == pid_child) {
//            set_terminal(pid_child);
//        }
    }
}

void run_parent(pid_t pid_fork, char* args[], int background) {
    int pid_wait; /* pid for created and waited process */
    int status;             /* status returned by wait */
    enum status status_res; /* status processed by analyze_status() */
    int info;				/* info processed by analyze_status() */

    waiting_pid = pid_fork;

    job* new_job_created = new_job(pid_fork, args[0], background);

    new_process_group(pid_fork);
    add_job(global_jobs, new_job_created);

    if (!background) {

        /* FUNCIONAMIENTO PRINCIPAL */
        set_terminal(pid_fork); // le da el terminal al hijo
        pid_wait = waitpid(pid_fork, &status, WUNTRACED); // espera por el hijo, pid_wait devuelve el pid del hijo
        set_terminal(getpid()); // le da el terminal al padre
        /* ------------------------ */

        printf("pid_wait: %i, pid_fork %i son iguales %i\n", pid_wait, pid_fork, pid_wait == pid_fork);
        if (pid_wait == pid_fork) {
            status_res = analyze_status(status, &info);


            if (status_res == EXITED) {
                delete_job(global_jobs, new_job_created);
            }

            if (info != 255) {
                printf("%s pid: %i, command: %s, %s, info: %i\n",
                       state_strings[background],
                       pid_fork,
                       args[0],
                       status_strings[status_res],
                       info);
            }

        }

    } else {
        signal(SIGCHLD, manejador_SIGCHILD);
        printf("%s job running... pid: %i, command: %s\n", state_strings[background], pid_fork, args[0]);
    }
}

void run_child(char* args[]) {
    restore_terminal_signals();
    execvp(args[0], args);
    fprintf(stderr, "Error, command not found: %s\n", args[0]);
    exit(-1);
}

int run_interal_commands(char* args[]) {
    int has_runned = 0;
    if (!strcmp(args[0], "cd")) {
        chdir(args[1]);
        has_runned = 1;
    } else if (!strcmp(args[0], "exit")) {
        exit(0);
    } else if (!strcmp(args[0], "jobs")) {
        if (!empty_list(global_jobs)) {
            print_job_list(global_jobs);
        }
        has_runned = 1;
    } else if (!strcmp(args[0], "mfg")) {
        fg(args[1]);
        has_runned = 1;
    } else if (!strcmp(args[0], "mbg")) {
        bg(args[1]);
        has_runned = 1;
    }
    return has_runned;
}

int main(void)
{
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background;             /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/2];     /* command line (of 256) has max of 128 arguments */
	// probably useful variables:

	global_jobs = new_list("jobs");

    ignore_terminal_signals();

    while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{
        printf("COMMAND->");
        fflush(stdout);
        get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */

        if(args[0]==NULL) continue;   // if empty command

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

    /* the steps are:
         (1) fork a child process using fork()
         (2) the child process will invoke execvp()
         (3) if background == 0, the parent will wait, otherwise continue
         (4) Shell shows a status message for processed command
         (5) loop returns to get_commnad() function
    */
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