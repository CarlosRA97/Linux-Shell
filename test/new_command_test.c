#include "../history/history.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>



void test_inserting_into_args(command_used *com, char **args, int *background) {
    strcpy(args[0], com->command);

    if (com->args[0] == NULL) {
        args[1] = NULL;
    } else {
        for (int i = 0; (com->args)[i] != NULL; i++) {
            strcpy(args[i + 1], (com->args)[i]);
        }
    }

    *background = com->background;
}


int main(int argc, char * argv[]) {
    if (argc > 1) {
        char * command = argv[1];
        char * args[argc];
        for (int i = 0; i < argc; i++) {
            args[i] = argv[i+1];
        }

        printf("Checking if new_command works properly\n");
        command_used * com = new_command(command, args, 0);

        printf("command (%s)\n", com->command);
        for (int i = 0; (com->args)[i] != NULL; i++) {
            printf("arg %d (%s)\n", i, (com->args)[i]);
        }

        print_command(com);


        printf("Checking if insert_command_to_args works properly\n");
        int background;

        insert_command_to_args(com, args, &background);

        printf("command copied %s\n", args[0]);
        for (int i = 1; args[i] != NULL; i++) {
            printf("arg (%d: %s)\n", i, args[i]);
        }
        printf("background (%d)\n", background);

    }
    return 0;
}

