//
// Created by Carlos Rivero Aro on 2019-06-11.
//
#include "../internal_commands/internal_commands.h"
#include "common_test.h"

int main(int argc, char ** argv) {
    if (argc > 2) {
        char * args[argc];
        parser(argc, argv, args);
        printf("times %d\n", team(args));
        for (int i = 0; args[i] != NULL; i++) {
            printf("%s ", args[i]);
        }
        printf("\n");
    }
}