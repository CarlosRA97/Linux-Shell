#include "pipe/pipe.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char * argv[]) {
    char * args[argc];
    for (int i = 0; i < argc; i++) {
        args[i] = argv[i+1];
    }

    printf("%d, is equal %d\n", find_pipe(args), !strcmp(argv[find_pipe(args) + 1], "|"));

}