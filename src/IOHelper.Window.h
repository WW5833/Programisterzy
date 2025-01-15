#ifndef _INC_IOHELPER_WINDOW_H
#define _INC_IOHELPER_WINDOW_H

#include <stdbool.h>
#include "IOHelper.h"
#include <windows.h>

/// @brief Process a window resize event
/// @param wbsr The window buffer size record
void ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr);
/// @brief The window IO loop
void Window_IOLoop();
/// @brief Enqueue a resize event call
void EnqueuResizeEventCall();

#endif // _INC_IOHELPER_WINDOW_H
