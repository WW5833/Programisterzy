#include "IOHelper.Window.h"
#include <stdio.h>
#include <time.h>
#include "AnsiHelper.h"

extern bool internal_IOHelper_LoopLock;

#define MAX_Y_SIZE_CONSOLE 150

void (*resizeHandler)(void *) = NULL;
void *resizeHandlerData = NULL;
void SetResizeHandler(void (*handler)(void *), void *data)
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
void CallResizeHandler()
{
    if(internal_IOHelper_LoopLock) return;

    if(resizeHandler == NULL || !resizeCallPending) return;

    if(difftime(time(NULL), lastResizeEvent) < 0.10) {
        return;
    }

    resizeCallPending = false;
    internal_IOHelper_LoopLock = true;
    resizeHandler(resizeHandlerData);
    internal_IOHelper_LoopLock = false;
}

void GetTerminalSize(int *x, int *y)
{
    internal_IOHelper_LoopLock = true;
    SetCursorPosition(999, 999);
    GetCursorPosition(x, y);
    internal_IOHelper_LoopLock = false;
}

void UpdateTerminalSize()
{
    GetTerminalSize(&LatestTerminalWidth, &LatestTerminalHeight);
}

extern bool internal_IO_WaitingResizeEvent;

void EnqueuResizeEventCall() {
    resizeCallPending = true;
}

void ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD wbsr)
{
    int termianlWidth = wbsr.dwSize.X;
    int terminalHeight = wbsr.dwSize.Y;

    if(wbsr.dwSize.Y > MAX_Y_SIZE_CONSOLE) { // Cmd has always height of buffer in resize info but size check will return true value
        if(internal_IOHelper_LoopLock) {
            terminalHeight = LatestTerminalHeight; // Prevent infinite loop
        }
        else {
            GetTerminalSize(&termianlWidth, &terminalHeight);
        }
    }

    LatestTerminalWidth = termianlWidth;
    LatestTerminalHeight = terminalHeight;

    internal_IO_WaitingResizeEvent = false;

    if(resizeHandler == NULL) return;

    EnqueuResizeEventCall();
    lastResizeEvent = time(NULL);
}

void Window_IOLoop()
{
    CallResizeHandler();
}
