#include "../history/history.h"
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char * argv[]) {
    if (argc > 1) {
        char * command = argv[1];
        char * args[argc];
        for (int i = 0; i < argc; i++) {
            args[i] = argv[i+1];
        }
        
        command_used * com = new_command(command, args, 0);

        printf("command (%s)\n", com->command);
        for (int i = 0; (com->args)[i] != NULL; i++) {
            printf("arg %d (%s)\n", i, (com->args)[i]);
        }
    }
    return 0;
}