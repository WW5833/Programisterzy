#ifndef _INC_IOHELPER_KEYBOARD_H
#define _INC_IOHELPER_KEYBOARD_H

#include <stdbool.h>
#include "IOHelper.h"
#include <windows.h>

/// @brief Process a key event
/// @param ker The key event record
void KeyEventProc(KEY_EVENT_RECORD ker);

#endif // _INC_IOHELPER_KEYBOARD_H
