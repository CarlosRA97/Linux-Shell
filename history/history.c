
/*============================================================*/
/* lee un caracter sin esperar a '\n' y sin eco */
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include "history.h"

command_used * new_command(const char * command, char ** args, int background)
{
	command_used * aux;
	aux=(command_used *) malloc(sizeof(command_used));
	aux->command=strdup(command);
	if (args != NULL) {
		aux->args = (char**) malloc(sizeof(char*));
		int i = 1;
		while (args[i] != NULL) {
			aux->args[i-1] = (char*) malloc(sizeof(char));
			strcpy(aux->args[i-1], args[i]);
			i++;
		}
	} else {
		aux->args = NULL;
	}
	aux->background = background;
	aux->next=NULL;
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
	while(aux->next!= NULL && n) { aux=aux->next; n--;}
	return aux->next;
}

void print_command(command_used * item)
{
	printf("%s ", item->command);
	if (item->args != NULL) {
		int i = 0;
		printf("%s ", (item->args)[i] != NULL ? (item->args)[i] : "");
	}
	printf("%s\n", item->background ? "&" : "");
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