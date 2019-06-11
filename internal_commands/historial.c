#include "internal_commands.h"

int historial(command_used * historial_commands, char ** args, int * background) {
    int has_runned = 1;
    if (args[1] == NULL) {
        print_command_history_list(historial_commands);
    } else {
        int position = atoi(args[1]);
        command_used* com = get_command_used_bypos(historial_commands, position);
        if (com != NULL) {
            insert_command_to_args(com, args, background);
            has_runned = 0;
        } else {
            fprintf(stderr, "Tas pasao\n");
        }
    }
    return has_runned;
}