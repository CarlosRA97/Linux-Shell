#ifndef _CURSOR_H
#define _CURSOR_H

void save_cursor(int * cursorPos, int * cursorBackup);
void restore_cursor(int * cursorPos, int * cursorBackup);

#endif