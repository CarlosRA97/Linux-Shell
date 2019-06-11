//
// Created by Carlos Rivero Aro on 2019-06-11.
//
#include <string.h>
#include <stdlib.h>

int team(char ** args) {
    int times = 1;
    if (!strcmp(args[0], "team") && args[1] != NULL && args[2] != NULL) {
        times = atoi(args[1]);
        int i = 0;
        for (; args[i + 2] != NULL; i++) {
            args[i] = args[i + 2];
            args[i + 2] = NULL;
        }
        args[i] = NULL;

    }
    return times;
}