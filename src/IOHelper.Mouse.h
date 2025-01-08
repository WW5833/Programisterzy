#ifndef _INC_IOHELPER_MOUSE_H
#define _INC_IOHELPER_MOUSE_H

#include <stdbool.h>
#include "IOHelper.h"
#include <windows.h>

void MouseEventProc(MOUSE_EVENT_RECORD);
void Mouse_IOLoop();

#endif // _INC_IOHELPER_MOUSE_H
