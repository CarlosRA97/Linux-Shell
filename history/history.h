#ifndef SHELL_HISTORY_H
#define SHELL_HISTORY_H

typedef struct command_used_
{
    char *command; /* program name */
    char **args;
    int background;
    int length;
    struct command_used_ *next; /* next command usedin the list */
} command_used;

command_used *new_command(const char *command, char ** args, int background);

command_used * get_command_used_bypos( command_used * list, int n);
void add_command_used(command_used *list, command_used *item);
void print_command(command_used *item);
void print_command_list(command_used *list, void (*print)(command_used *));
void execute_command(command_used * com);

#define print_command_history_list(list)    print_command_list(list, print_command)
#define new_historial_list() 	            new_command("historial", NULL, 0)

#endif