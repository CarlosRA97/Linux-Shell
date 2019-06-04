#include "internal_commands.h"

int historial(command_used * historial_commands, char ** args, int * background) {
    int has_runned = 1;
    if (args[1] == NULL) {
        print_command_history_list(historial_commands);
    } else {
        command_used* com = get_command_used_bypos(historial_commands, atoi(args[1]));
        if (com != NULL) {
            strcpy(args[0], com->command);
            
            if (com->args[0] == NULL) {
                args[1] = NULL;
            } else {
                for (int i = 0; (com->args)[i] != NULL; i++) {
                    strcpy(args[i+1], strdup((com->args)[i]));
                }
            }
            
            *background = com->background;
            has_runned = 0;
        } else {
            fprintf(stderr, "Tas pasao\n");
        }
    }
    return has_runned;
}