#include "../pipe/pipe.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char * argv[]) {
    if (argc > 1) {
        char * args[argc];
        for (int i = 0; i < argc; i++) {
            args[i] = argv[i+1];
        }
        int pipes[6];
        int pipeAtEnd = 0;
        for (int i = 0; i < 6; i++) {
            pipes[i] = 0;
        }
        find_pipes(args, pipes, &pipeAtEnd);
        for (int i = 0; i < 6 && pipes[i] != 0; i++) {
            printf("%d\n", pipes[i]);
        }
        printf("pipeAtEnd? %s\n", pipeAtEnd ? "yes" : "no");
        for (int i = 0; args[i] != NULL; i++) {
            if (args[i+pipeAtEnd] != NULL) {
                printf("%s ", args[i]);
            }
        }
        printf("\n");
    }
    return 0;
}