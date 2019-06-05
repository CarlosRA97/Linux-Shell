#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "../common/common.h"

int find_pipe(char * args[]) {
	int pipePosition = 0;
	while (args[pipePosition] != NULL && pipePosition < MAX_LINE/2 && strcmp(args[pipePosition], "|")) pipePosition++;
	return args[pipePosition] != NULL && pipePosition < MAX_LINE/2 ? pipePosition : 0;
}

void split_args(int pipePosition, char * args[], char * args1[], char * args2[]) {
	if (pipePosition) {
		int args1pos = 0;
		for (int i = 0; i < pipePosition; i++) {
			args1[args1pos++] = args[i];
		}
		args1[args1pos] = NULL;

		int args2pos = 0;
		for (int i = pipePosition + 1; i < MAX_LINE/2 && args[i] != NULL; i++) {
			args2[args2pos++] = args[i];
		}
		args2[args2pos] = NULL;
	}
}

void exec_write_pipe(char * args[], int descf[], int * fno) {

	/* el proceso padre ejecuta el primer programa y cambia su
	salida estandar al pipe cerrando la entrada del pipe */
	*fno=fileno(stdout);
	dup2(descf[1], *fno);
	close(descf[0]);
	execvp(args[0], args);
}

void exec_read_pipe(char * args[], int descf[], int * fno) {

	/* el proceso hijo tiene una copia del pipe del padre,
	en el fork, ejecuta el segundo programa y cambia su
	entrada estandar por el pipe cerrando la salida del pipe */
	*fno=fileno(stdin);
	dup2(descf[0], *fno);
	close(descf[1]);
	execvp(args[0], args);
}

void exec_pipe(char * args[]) {
	int descf[2], fno;
	pipe(descf); /* se crea un pipe */
	int pid_fork = fork();
	if (pid_fork)
	{
		/* el proceso padre ejecuta el primer programa y cambia su
		salida estandar al pipe cerrando la entrada del pipe */
		fno=fileno(stdout);
		dup2(descf[1], fno);
		close(descf[0]);
		execlp(args[0], args[0], NULL);
	}
	else
	{
		/* el proceso hijo tiene una copia del pipe del padre,
		en el fork, ejecuta el segundo programa y cambia su
		entrada estandar por el pipe cerrando la salida del pipe */
		fno=fileno(stdin);
		dup2(descf[0], fno);
		close(descf[1]);
		execlp(args[2], args[2], NULL);
	}
}
