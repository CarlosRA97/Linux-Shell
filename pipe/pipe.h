#ifndef _PIPE_H
#define _PIPE_H

void exec_pipe(char * args[]);

void exec_pipe_child1(char * args[], int descf[], int * fno);
void exec_pipe_child2(char * args[], int descf[], int * fno);

int find_pipe(char * args[]);
void split_args(int pipePosition, char * args[], char * args1[], char * args2[]);

#endif