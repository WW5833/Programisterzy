#include "IOHelper.ManualHandling.Window.h"
#include <stdio.h>
#include <time.h>
#include "AnsiDefinitions.h"
#include "DebugCheck.h"

#ifdef PROGRAMISTERZY_EXTENDED_TERMINAL_INTEGRATION

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

int latestTerminalWidth = -1;
int latestTerminalHeight = -1;
time_t lastResizeCall = 0;
bool resizeCallPending = false;
void CallResizeHandler(int width, int height)
{
    if(IOHelper_LoopLock || internal_IOHelper_LoopLock) return;

    if(resizeHandler == NULL || !resizeCallPending) return;

    if(difftime(time(NULL), lastResizeCall) < 0.25) {
        return;
    }

    lastResizeCall = time(NULL);
    resizeCallPending = false;
    internal_IOHelper_LoopLock = true;
    resizeHandler(width, height, resizeHandlerData);
    internal_IOHelper_LoopLock = false;
}

void ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
    if(resizeHandler == NULL) return;

    int termianlWidth = wbsr.dwSize.X;
    int terminalHeight = wbsr.dwSize.Y;

    if(wbsr.dwSize.Y > MAX_Y_SIZE_CONSOLE) {
        GetTerminalSize(&termianlWidth, &terminalHeight);
    }

    latestTerminalWidth = termianlWidth;
    latestTerminalHeight = terminalHeight;

    resizeCallPending = true;
    CallResizeHandler(termianlWidth, terminalHeight);
}

void Window_IOLoop()
{
    CallResizeHandler(latestTerminalWidth, latestTerminalHeight);
}

#endif
