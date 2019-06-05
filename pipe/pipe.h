#ifndef _PIPE_H
#define _PIPE_H

void exec_pipe(char * args[]);

void exec_write_pipe(char * args[], int descf[], int * fno);
void exec_read_pipe(char * args[], int descf[], int * fno);

int find_pipe(char * args[]);
void find_pipes(char * args[], int pipesPos[], int * pipeAtEnd);
void split_args(int pipePosition, char * args[], char * args1[], char * args2[]);
void close_pipe(int descf[]);

#endif