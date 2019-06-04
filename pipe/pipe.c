#include <unistd.h>
#include <stdio.h>

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
