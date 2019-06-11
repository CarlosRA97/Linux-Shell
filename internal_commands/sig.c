#include "internal_commands.h"
#include <stdio.h>

int signaled;

void sig() {
    printf("Signaleds %d\n", signaled);
}

void sig_inc() {
    signaled++;
}