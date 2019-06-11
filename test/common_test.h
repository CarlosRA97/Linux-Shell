//
// Created by Carlos Rivero Aro on 2019-06-11.
//

#ifndef SHELL_COMMON_TEST_H
#define SHELL_COMMON_TEST_H

void parser(int argc, char ** argv, char ** args) {

    for (int i = 0; i < argc; i++) {
        args[i] = argv[i+1];
    }

}

#endif //SHELL_COMMON_TEST_H
