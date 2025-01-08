#include "IOHelper.Window.h"
#include <stdio.h>
#include <time.h>
#include "AnsiDefinitions.h"

extern bool IOHelper_LoopLock;
extern bool internal_IOHelper_LoopLock;

#define MAX_Y_SIZE_CONSOLE 150

void (*resizeHandler)(int, int, void *) = NULL;
void *resizeHandlerData = NULL;
void SetResizeHandler(void (*handler)(int, int, void *), void *data)
{
    resizeHandler = handler;
    resizeHandlerData = data;
}

void UnsetResizeHandler()
{
    resizeHandler = NULL;
    resizeHandlerData = NULL;
}

int LatestTerminalWidth = -1;
int LatestTerminalHeight = -1;

time_t lastResizeEvent = 0;
bool resizeCallPending = false;
void CallResizeHandler(int width, int height)
{
    if(IOHelper_LoopLock || internal_IOHelper_LoopLock) return;

    if(resizeHandler == NULL || !resizeCallPending) return;

    if(difftime(time(NULL), lastResizeEvent) < 0.10) {
        return;
    }

    resizeCallPending = false;
    internal_IOHelper_LoopLock = true;
    resizeHandler(width, height, resizeHandlerData);
    internal_IOHelper_LoopLock = false;
}

void ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
    int termianlWidth = wbsr.dwSize.X;
    int terminalHeight = wbsr.dwSize.Y;

    if(wbsr.dwSize.Y > MAX_Y_SIZE_CONSOLE) { // Cmd has always height of buffer in resize info but size check will return true value
        GetTerminalSize(&termianlWidth, &terminalHeight);
    }

    LatestTerminalWidth = termianlWidth;
    LatestTerminalHeight = terminalHeight;

    if(resizeHandler == NULL) return;

    resizeCallPending = true;
    lastResizeEvent = time(NULL);
}

void Window_IOLoop()
{
    CallResizeHandler(LatestTerminalWidth, LatestTerminalHeight);
}
