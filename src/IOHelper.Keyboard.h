#ifndef _INC_IOHELPER_KEYBOARD_H
#define _INC_IOHELPER_KEYBOARD_H

#include <stdbool.h>
#include "IOHelper.h"
#include <windows.h>

void KeyEventProc(KEY_EVENT_RECORD);
void IOLoop();

#endif // _INC_IOHELPER_KEYBOARD_H
