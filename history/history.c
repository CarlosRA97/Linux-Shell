
/*============================================================*/
/* lee un caracter sin esperar test_inserting_into_args '\n' y sin eco */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#ifndef __APPLE__
#include <malloc.h>
#endif
#include <string.h>
#include "history.h"
#include "../common/common.h"

command_used * new_command(const char * command, char * args[], int background)
{
	command_used * aux = (command_used *) calloc(1, sizeof(command_used));
	aux->command = strdup(command);
	if (args != NULL) {
		aux->args = (char**) calloc(MAX_LINE/2, sizeof(char*));
		for (int i = 0; args[i+1] != NULL; i++) {
			aux->args[i] = (char*) calloc(1, sizeof(char));
			strcpy(aux->args[i], args[i+1]);
		}
	}
	aux->background = background;
	return aux;
}

void add_command_used (command_used * list, command_used * item)
{
	command_used * aux=list->next;
	list->next=item;
	item->next=aux;
	list->length++;
}

command_used * get_command_used_bypos (command_used * list, int n)
{
	command_used * aux=list;
	if(n<1 || n>list->length) return NULL;
	n--;
	while(aux->next != NULL && n) { aux=aux->next; n--; }
	return aux->next;
}

void print_command(command_used * item)
{
	printf("%s ", item->command);
	if (item->args[0] != NULL) {
		for (int i = 0; item->args[i] != NULL; i++) {
			printf("%s ", item->args[i]);
		}
	}
	printf("%s\n", item->background ? "&" : "");
}

void insert_command_to_args(command_used * item, char ** args, int * background)
{

    strcpy(args[0], item->command);

    int i = 1;
    for (; item->args[i - 1] != NULL ; i++) {
        args[i] = item->args[i - 1];
    }
    args[i] = NULL;

    *background = item->background;

}

void print_command_list(command_used * list, void (*print)(command_used *))
{
	int n=1;
	command_used * aux=list;
	while(aux->next!= NULL)
	{
		printf(" %d ",n);
		print(aux->next);
		n++;
		aux=aux->next;
	}
}