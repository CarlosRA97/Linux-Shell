#include "internal_commands.h"
#include <stdlib.h>
#include <stdio.h>

void cd(char * arg) {
    int exitCode = chdir(arg);
    if (arg != NULL && exitCode == -1) {
        fprintf(stderr, "No se puede cambiar al directorio %s\n", arg);
        fflush(stdout);
    }
}