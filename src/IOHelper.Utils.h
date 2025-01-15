#ifndef _INC_IOHELPER_UTILS_H
#define _INC_IOHELPER_UTILS_H

#include <stdbool.h>

#define ENTER '\015'
#define ESC '\033'
#define ANY_MOUSE_BUTTON (char)0xFF
#define RESIZE_EVENT (char)0xFE
#define ESCAPE_CHAR (char)(224)

/// @brief The type of key input
typedef enum
{
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

/// @brief Handle interactions
/// @param blocking Whether to block until a key is pressed
/// @return The key input type
KeyInputType HandleInteractions(bool blocking);

/// @brief Wait for keys
/// @param count The number of types of keys to wait for
/// @param keys The keys to wait for
/// @return The key pressed
char _internal_WaitForKeys(int count, char* keys);
/// @brief Wait for keys
/// @param ... The keys to wait for
#define WaitForKeys(...) _internal_WaitForKeys(sizeof((char[]){__VA_ARGS__}), (char[]){__VA_ARGS__})

#endif // _INC_IOHELPER_UTILS_H
