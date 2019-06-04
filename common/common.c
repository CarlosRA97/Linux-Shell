#include "common.h"

void print_info(int state, pid_t pid, char *command, enum status status_res, int info) {
    printf("%s pid: %i, command: %s, %s, info: %i\n",
           state_strings[state],
           pid,
           command,
           status_strings[status_res],
           info);
}