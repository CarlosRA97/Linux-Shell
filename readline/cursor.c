#include "cursor.h"

void save_cursor(int * cursorPos, int * cursorBackup) {
    *cursorBackup = *cursorPos;
}

void restore_cursor(int * cursorPos, int * cursorBackup) {
    *cursorPos = *cursorBackup;
}

void reset_cursor(int cursorPos) {
    moveLeft(cursorPos - 1);
    save();
}