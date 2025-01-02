#ifndef _INC_PAGEUTILS_H
#define _INC_PAGEUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define CTRL_C '\03'
#define ENTER '\015'

int GetWrappedLineCount(const char* line, int width);
void PrintWrappedLine(const char* line, int width, int secondaryOffset, bool centerText);
void GetWrappedLineCursorPosition(const char* line, int width, int position, int* cursorX, int* cusrorY);

typedef enum {
    KEY_NONE = 0,
    KEY_ARROW_UP = 1,
    KEY_ARROW_DOWN = 2,
    KEY_ARROW_RIGHT = 3,
    KEY_ARROW_LEFT = 4,
    KEY_ENTER = 5,
    KEY_X = 6,
    KEY_Y = 7,
    KEY_Z = 8,
    KEY_R = 9,
} KeyInputType;

KeyInputType HandleInteractions(bool blocking);

void ExitOnCtrlC();
void WaitForEnter();

#endif // _INC_PAGEUTILS_H