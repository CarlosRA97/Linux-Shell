
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#ifndef __APPLE__
#include <malloc.h>
#endif

#include "readline.h"
#include "cursor.h"

#include "../history/history.h"

char getch()
{
    int shell_terminal = STDIN_FILENO;
    struct termios conf;
    struct termios conf_new;
    char c;

    tcgetattr(shell_terminal,&conf); /* leemos la configuracion actual */
    conf_new = conf;

    conf_new.c_lflag &= (~(ICANON|ECHO)); /* configuramos sin buffer ni eco */
    conf_new.c_cc[VTIME] = 0;
    conf_new.c_cc[VMIN] = 1;

    tcsetattr(shell_terminal,TCSANOW,&conf_new); /* establecer configuracion */

    c = getc(stdin); /* leemos el caracter */

    tcsetattr(shell_terminal,TCSANOW,&conf); /* restauramos la configuracion */
    return c;
}

void str_to_buf(char inputBuffer[], char word[], int length, int * offset) {
    for (int i = 0; i < length; i++) {
        inputBuffer[*offset + i] = word[i];
    }
    *offset += length;
}

int putCommandInto(char inputBuffer[], command_used * historial, int pos) {
    command_used* com = get_command_used_bypos(historial, pos);
    if (com != NULL) {
        int cursor = 0;
        str_to_buf(inputBuffer, com->command, strlen(com->command), &cursor);

        if (com->args != NULL) {
            for (int i = 0; (com->args)[i] != NULL; i++) {
                str_to_buf(inputBuffer, " ", strlen(" "), &cursor);
                str_to_buf(inputBuffer, (com->args)[i], strlen((com->args)[i]), &cursor);
            }
        }
        char * background = com->background ? " &" : "";
        str_to_buf(inputBuffer, background, strlen(background), &cursor);
        return cursor;
    } else {
        return 0;
    }
}

void printCommand(char inputBuffer[], int size, int * length, command_used * historial, int cursorHistory) {
    delete(size);
    int charsread = putCommandInto(inputBuffer, historial, cursorHistory);
    if (charsread) {
        printf("%s", inputBuffer);
        *length = charsread;
    }
}

void clear_inputBuffer(char inputBuffer[], int size) {
    for (int i = 0; i < size; i++) {
        inputBuffer[i] = '\0';
    }
}

void save_written_command(char inputBuffer[], char writenCommand[]) {
    strcpy(writenCommand, inputBuffer);
}

void restore_writen_command(char inputBuffer[], char writenCommand[]) {
    strcpy(inputBuffer, writenCommand);
}

void show_command(char inputBuffer[], int size, int *length, command_used * historial, int cursorHistory, int * cursorPos) {
    restore();
                                
    clear_inputBuffer(inputBuffer, size);

    printCommand(inputBuffer, size, length, historial, cursorHistory);
    *cursorPos = *length;
}

void writeRight(char inputBuffer[], int size, int pos, char aux[]) {

    inputBuffer[pos] = aux[pos-1];
    printf("%c", inputBuffer[pos]);

    for (int i = pos+1; i < size; i++)
    {
        inputBuffer[i] = aux[i-1];
        printf("%c", inputBuffer[i]);
    }
    
}

void shiftLeft(char inputBuffer[], int size, int pos) {
    for (int i = pos; i < size; i++) {
        inputBuffer[i] = inputBuffer[i+1];
    }
}

int readline(char inputBuffer[], int size, command_used * historial) 
{
    /* Las teclas de cursor devuelven una secuencia de 3 caracteres,
    27 -­­ 91 -­­ (65, 66, 67 ó 68) */
    bool EOL = false;
    int length = 0;
    int cursorPos = 0;
    int cursorBackup = 0;

    char writenCommand[size];
    bool writen = false;

    char inputBufferAux[size];
    clear_inputBuffer(inputBufferAux, size);

    int lengthHistorial = historial->length;
    int cursorHistory = 0;
    
    clear_inputBuffer(inputBuffer, size);
    save();

    while (!EOL) {
        char sec[3];
        sec[0] = getch();
        // printf("%d", sec[0]);
        switch (sec[0])
        {
            case ESC:
                sec[1] = getch();
                if (sec[1] == LEFT_SQUARE_BRACKET) {
                    sec[2] = getch();
                    switch (sec[2])
                    {
                        case KEY_UP: /* ARRIBA */
                            if (cursorHistory < lengthHistorial) {
                                cursorHistory++;

                                reset_cursor(cursorPos);

                                if (!writen) {
                                    save_written_command(inputBuffer, writenCommand);
                                    writen = true;
                                }
                                
                                show_command(inputBuffer, size, &length, historial, cursorHistory, &cursorPos);
                            }
                        break;
                        case KEY_DOWN: /* ABAJO */
                            if (cursorHistory > 1) {
                                cursorHistory--;

                                reset_cursor(cursorPos);
                                
                                show_command(inputBuffer, size, &length, historial, cursorHistory, &cursorPos);
                            } 
                            else {
                                if (writen) {
                                    reset_cursor(cursorPos);

                                    clear_inputBuffer(inputBuffer, size);
                                    
                                    delete(size);

                                    restore_writen_command(inputBuffer, writenCommand);

                                    printf("%s", inputBuffer);
                                    cursorPos = strlen(inputBuffer);
                                    cursorHistory--;
                                    writen = false;
                                }
                            }   
                        break;
                        case KEY_RIGHT: /* DERECHA */
                            if (cursorPos < length) {
                                cursorPos++;
                                moveRight(1);
                            }
                        break;
                        case KEY_LEFT: /* IZQUIERDA */
                            if (cursorPos > 0) {
                                cursorPos--;
                                moveLeft(1);
                            }
                        break;
                        default:
                        break;
                    }
                }
                break;
            case CtrlL:
                EOL = true;
                length++;
                clear();
                break;
            case CtrlD: 
                EOL = true;
                length = 0;
                break;
            case ENTER:
                EOL = true;
                if (cursorPos == 0) {
                    inputBuffer[0] = '\0';
                    length++;
                } else {
                    str_to_buf(inputBuffer, "\n", strlen("\n"), &length);
                }
                printf("\n");
                break;
            case DELETE: /* BORRAR */
                if (cursorPos > 0) {
                    length--;
                    cursorPos--;
                    shiftLeft(inputBuffer, size, cursorPos);
                    moveLeft(1);
                    delete(1);
                }
                break;
            case TAB:
            break;
            default:
                strcpy(inputBufferAux, inputBuffer);
                inputBuffer[cursorPos] = sec[0];
                printf("%c", inputBuffer[cursorPos]);
                save();
                
                length++;
                cursorPos++;

                if (cursorPos != length) {
                    delete(size);
                    writeRight(inputBuffer, size, cursorPos, inputBufferAux);
                }

                restore();
                
                break;
        }
    }
    return length;
}
