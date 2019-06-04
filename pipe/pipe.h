#ifndef _PIPE_H
#define _PIPE_H

void exec_pipe(char * args[]);

void exec_pipe_child(char * args[]);
void exec_pipe_parent(char * args[]);

#endif