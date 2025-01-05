#ifndef _INC_PAGEUTILS_H
#define _INC_PAGEUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define CTRL_C '\03'
#define ENTER '\015'
#define ESC '\033'

const char* GetNextChar(const char* c);
int GetStringCharCount(const char* start);
int GetCharCount(const char* start, const char* end);
int GetWrappedLineCount(const char* line, int width);
void PrintWrappedLine(const char* line, int width, int secondaryOffset, bool centerText);

typedef enum {
    KEY_NONE = 0,
    KEY_ARROW_UP,
    KEY_ARROW_DOWN,
    KEY_ARROW_RIGHT,
    KEY_ARROW_LEFT,
    KEY_ENTER,
    KEY_ESCAPE,
    KEY_1,
    KEY_2,
    KEY_3,
    KEY_R,
} KeyInputType;

KeyInputType HandleInteractions(bool blocking);

void ExitOnCtrlC();
void WaitForEnter();
char _internal_WaitForKeys(int count, char* keys);
#define WaitForKeys(...) _internal_WaitForKeys(sizeof((char[]){__VA_ARGS__}), (char[]){__VA_ARGS__})

#endif // _INC_PAGEUTILS_H