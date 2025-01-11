#ifndef _INC_PAGEUTILS_H
#define _INC_PAGEUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ENTER '\015'
#define ESC '\033'
#define ANY_MOUSE_BUTTON (char)0xFF
#define RESIZE_EVENT (char)0xFE

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

void WaitForEnter();
char _internal_WaitForKeys(int count, char* keys);
#define WaitForKeys(...) _internal_WaitForKeys(sizeof((char[]){__VA_ARGS__}), (char[]){__VA_ARGS__})

#endif // _INC_PAGEUTILS_H
