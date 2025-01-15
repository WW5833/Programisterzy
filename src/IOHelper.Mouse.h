#ifndef _INC_IOHELPER_MOUSE_H
#define _INC_IOHELPER_MOUSE_H

#include <stdbool.h>
#include "IOHelper.h"
#include <windows.h>

/// @brief Process a mouse event
/// @param mer The mouse event record
void MouseEventProc(MOUSE_EVENT_RECORD mer);
/// @brief The mouse IO loop
void Mouse_IOLoop();

#endif // _INC_IOHELPER_MOUSE_H
