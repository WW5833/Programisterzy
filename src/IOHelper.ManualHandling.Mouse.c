#include "IOHelper.ManualHandling.Mouse.h"
#include <stdio.h>
#include "DebugCheck.h"

#ifdef PROGRAMISTERZY_EXTENDED_TERMINAL_INTEGRATION

#define MER_QUEUE_SIZE 64
MOUSE_EVENT_RECORD merQueue[MER_QUEUE_SIZE];
int merQueueReadPtr = 0;
int merQueueWritePtr = 0;
#define MerBufferPtrInc(ptr) ptr++; if(ptr >= MER_QUEUE_SIZE) ptr = 0

extern bool IOHelper_LoopLock;
extern bool internal_IOHelper_LoopLock;

void (*mouseClickHandler)(int, int, int, void *) = NULL;
void (*mouseDoubleClickHandler)(int, int, int, void *) = NULL;
void (*mouseScrollHandler)(bool, int, int, void *) = NULL;
void (*mouseMoveHandler)(int, int, void *) = NULL;
void *mouseHandlerData = NULL;
void SetMouseHandler(
    void (*clickHandler)(int, int, int, void *),
    void (*doubleClickHandler)(int, int, int, void *),
    void (*scrollHandler)(bool, int, int, void *),
    void (*moveHandler)(int, int, void *),
    void *data)
{
    EnableMouseInput(true);

    mouseClickHandler = clickHandler;
    mouseDoubleClickHandler = doubleClickHandler;
    mouseScrollHandler = scrollHandler;
    mouseMoveHandler = moveHandler;

    mouseHandlerData = data;
}

void UnsetMouseHandler()
{
    mouseClickHandler = NULL;
    mouseDoubleClickHandler = NULL;
    mouseScrollHandler = NULL;
    mouseMoveHandler = NULL;

    mouseHandlerData = NULL;

    EnableMouseInput(false);
}

void MouseEventProc(MOUSE_EVENT_RECORD mer)
{
    if(internal_IOHelper_LoopLock) {
        MerBufferPtrInc(merQueueWritePtr);
        merQueue[merQueueWritePtr] = mer;
        return;
    }

    do {
        internal_IOHelper_LoopLock = true;
        switch(mer.dwEventFlags)
        {
            case 0:
                if(mouseClickHandler != NULL) {
                    mouseClickHandler((int)mer.dwButtonState, mer.dwMousePosition.X, mer.dwMousePosition.Y, mouseHandlerData);
                }
                break;
            case DOUBLE_CLICK:
                if(mouseDoubleClickHandler != NULL) {
                    mouseDoubleClickHandler((int)mer.dwButtonState, mer.dwMousePosition.X, mer.dwMousePosition.Y, mouseHandlerData);
                }
                break;
            case MOUSE_WHEELED:
                if(mouseScrollHandler != NULL) {
                    mouseScrollHandler((mer.dwButtonState & 0x80000000) != 0, mer.dwMousePosition.X, mer.dwMousePosition.Y, mouseHandlerData);
                }
                break;
            case MOUSE_MOVED:
                if(mouseMoveHandler != NULL) {
                    mouseMoveHandler(mer.dwMousePosition.X, mer.dwMousePosition.Y, mouseHandlerData);
                }
                break;
        }
        internal_IOHelper_LoopLock = false;

        if(merQueueReadPtr == merQueueWritePtr) {
            break;
        }

        mer = merQueue[merQueueReadPtr];
        MerBufferPtrInc(merQueueReadPtr);
    }
    while(true);
}

#endif

void Mouse_IOLoop()
{
    if(internal_IOHelper_LoopLock) return;

    if(merQueueReadPtr != merQueueWritePtr) {
        MouseEventProc(merQueue[merQueueReadPtr]);
        MerBufferPtrInc(merQueueReadPtr);
    }
}
