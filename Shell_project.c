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

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */

// -----------------------------------------------------------------------
//                            MAIN          
// -----------------------------------------------------------------------

int main(void)
{
	char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
	int background;             /* equals 1 if a command is followed by '&' */
	char *args[MAX_LINE/2];     /* command line (of 256) has max of 128 arguments */
	// probably useful variables:
	int pid_fork, pid_wait; /* pid for created and waited process */
	int status;             /* status returned by wait */
	enum status status_res; /* status processed by analyze_status() */
	int info;				/* info processed by analyze_status() */
	job* jobs = new_list("jobs");

    ignore_terminal_signals();

    while (1)   /* Program terminates normally inside get_command() after ^D is typed*/
	{
        printf("COMMAND->");
        fflush(stdout);
        get_command(inputBuffer, MAX_LINE, args, &background);  /* get next command */

        if(args[0]==NULL) continue;   // if empty command

        /* the steps are:
             (1) fork a child process using fork()
             (2) the child process will invoke execvp()
             (3) if background == 0, the parent will wait, otherwise continue
             (4) Shell shows a status message for processed command
             (5) loop returns to get_commnad() function
        */

        if (!strcmp(args[0], "cd")) {
            chdir(args[1]);
            continue;
        } else if (!strcmp(args[0], "exit")) {
            exit(0);
        } else if (!strcmp(args[0], "jobs")) {
            if (!empty_list(jobs)) {
                print_job_list(jobs);
            }
            continue;
        }

        // (1)
        pid_fork = fork();

        new_process_group(pid_fork);

        job* new_job_created = new_job(pid_fork, args[0], background);

        add_job(jobs, new_job_created);

        int isChild = pid_fork == 0;

		if (isChild) {

		    if (!background) {
                set_terminal(getpid());
            } else {
                set_terminal(getppid());
            }
            restore_terminal_signals();

            execvp(args[0], args);
            fprintf(stderr, "Error, command not found: %s\n", args[0]);
            exit(-1);
            continue;
		} else { // Parent
		    // pid_fork == child pid

		    if (!background) {
                pid_wait = waitpid(pid_fork, &status, WUNTRACED); // pid_wait devuelve el pid del hijo
                set_terminal(getpid());
                status_res = analyze_status(status, &info);
                fflush(stdout);
                if (!info) {
                    delete_job(jobs, new_job_created);
                    printf("%s pid: %i, command: %s, %s, info: %i\n",
                           state_strings[background],
                           pid_fork,
                           args[0],
                           status_strings[status_res],
                           info);
                }
            } else {
                printf("%s job running... pid: %i, command: %s\n", state_strings[background], pid_fork, args[0]);
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

// waitpid(pid, &status, WNOHANG | WUNTRACED | WCONTINUED)
// pidw == pid -> mirar status (signaled, stopped, continued)
// borrar de la lista el proceso si signaled o stopped


// introducir en la lista de jobs cuando hay un proceso en segundo plano o un proceso en primer plano suspendido