#ifndef _CURSOR_H
#define _CURSOR_H

#include <stdio.h>

#define clear() printf("\033[H\033[J")
#define gotoxy(x,y) printf("\033[%d;%dH", (x), (y))
#define save() printf("\033[s")
#define restore() printf("\033[u")
#define delete(times) printf("\033[%dP", (times))
#define moveLeft(times) printf("\033[%dD", (times))
#define moveRight(times) printf("\033[%dC", (times))
#define moveCursor(pos) printf("\033[%da", pos)
#define erase() printf("\033[1K");

void save_cursor(int * cursorPos, int * cursorBackup);
void restore_cursor(int * cursorPos, int * cursorBackup);
void reset_cursor(int cursorPos);

#endif