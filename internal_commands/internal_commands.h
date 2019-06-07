#ifndef _INTERNAL_COMMANDS_H
#define _INTERNAL_COMMANDS_H

#include <string.h>

#include "../job/job_control.h"
#include "../history/history.h"
#include "../common/common.h"

int get_pos(job * global_jobs, char *arg);
void bg(job * global_jobs, char *arg);
void fg(job * global_jobs, char *arg);
void sig();
void cd(char *arg);
int historial(command_used * historial_commands, char ** args, int * background);
int run_interal_commands(job * global_jobs, command_used * historial_commands, char *args[], int * background);

#endif