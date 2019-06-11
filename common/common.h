#ifndef _COMMON_H
#define _COMMON_H

#include "../job/job_control.h"

#define ROJO "\x1b[31;1;1m"
#define NEGRO "\x1b[0m"
#define VERDE "\x1b[32;1;1m"
#define AZUL "\x1b[34;1;1m"
#define CIAN "\x1b[36;1;1m"
#define MARRON "\x1b[33;1;1m"
#define PURPURA "\x1b[35;1;1m"
#define $ "\e[0m"

#define MAX_LINE 256 /* 256 chars per line, per command, should be enough. */
#define maxChilds 2

void print_info(int state, pid_t pid, char *command, enum status status_res, int info);

job *global_jobs;
command_used *historial_commands;

#endif