#ifndef _INC_IOHELPER_UTILS_H
#define _INC_IOHELPER_UTILS_H

#include <stdbool.h>

#define ENTER '\015'
#define ESC '\033'
#define ANY_MOUSE_BUTTON (char)0xFF
#define RESIZE_EVENT (char)0xFE
#define ESCAPE_CHAR (char)(224)

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
} KeyInputType;

KeyInputType HandleInteractions(bool blocking);

char _internal_WaitForKeys(int count, char* keys);
#define WaitForKeys(...) _internal_WaitForKeys(sizeof((char[]){__VA_ARGS__}), (char[]){__VA_ARGS__})

#endif // _INC_IOHELPER_UTILS_H
