#include "internal_commands.h"
#include <stdio.h>

void sig() {
    printf("Signaleds %d\n", signaled);
}

void sig_inc() {
    signaled++;
}