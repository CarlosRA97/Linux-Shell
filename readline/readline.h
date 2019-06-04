
#ifndef _READLINE_H
#define _READLINE_H

#include "../history/history.h"
#include "cursor.h"

#define ESC 27
#define LEFT_SQUARE_BRACKET 91
#define KEY_UP 65
#define KEY_DOWN 66
#define KEY_RIGHT 67
#define KEY_LEFT 68
#define DELETE 127
#define ENTER 10
#define TAB 9
#define CtrlD 4
#define CtrlL 12

int readline(char inputBuffer[], int size, command_used * historial);

#endif