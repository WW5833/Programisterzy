#ifndef _INC_IOHELPER_WINDOW_H
#define _INC_IOHELPER_WINDOW_H

#include <stdbool.h>
#include "IOHelper.h"
#include <windows.h>

void ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD);
void Window_IOLoop();
void EnqueuResizeEventCall();

#endif // _INC_IOHELPER_WINDOW_H
