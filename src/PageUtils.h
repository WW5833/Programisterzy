#ifndef _INC_PAGEUTILS_H
#define _INC_PAGEUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define CTRL_C '\03'
#define ENTER '\015'
#define ESC '\033'
#define ESCAPE_CHAR 224

int GetStringCharCount(const char* start);
int GetCharCount(const char* start, const char* end);
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
    KEY_ESCAPE,
    KEY_1 = 6,
    KEY_2 = 7,
    KEY_3 = 8,
    KEY_R = 9,
} KeyInputType;

KeyInputType HandleInteractions(bool blocking);

void ExitOnCtrlC();
void WaitForEnter();
char _internal_WaitForKeys(int count, char* keys);
#define WaitForKeys(...) _internal_WaitForKeys(sizeof((char[]){__VA_ARGS__}), (char[]){__VA_ARGS__})

#endif // _INC_PAGEUTILS_H