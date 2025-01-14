#include "IOHelper.Mouse.h"
#include <stdio.h>

#define MER_QUEUE_SIZE 64
MOUSE_EVENT_RECORD merQueue[MER_QUEUE_SIZE];
int merQueueReadPtr = 0;
int merQueueWritePtr = 0;
#define MerBufferPtrInc(ptr) ptr++; if(ptr >= MER_QUEUE_SIZE) ptr = 0

extern bool internal_IOHelper_LoopLock;

void (*mouseClickHandler)(int, void *) = NULL;
void (*mouseDoubleClickHandler)(int, void *) = NULL;
void (*mouseScrollHandler)(bool, void *) = NULL;
void (*mouseMoveHandler)(void *) = NULL;
void *mouseHandlerData = NULL;
void SetMouseHandler(
    void (*clickHandler)(int, void *),
    void (*doubleClickHandler)(int, void *),
    void (*scrollHandler)(bool, void *),
    void (*moveHandler)(void *),
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

extern bool internal_IO_WaitingForMousePress;

int LatestMouseX = 0;
int LatestMouseY = 0;

void MouseEventProc(MOUSE_EVENT_RECORD mer)
{
    if(internal_IOHelper_LoopLock) {
        MerBufferPtrInc(merQueueWritePtr);
        merQueue[merQueueWritePtr] = mer;
        return;
    }

    do {
        internal_IOHelper_LoopLock = true;

        LatestMouseX = mer.dwMousePosition.X;
        LatestMouseY = mer.dwMousePosition.Y;

        switch(mer.dwEventFlags)
        {
            case 0:
                if(mer.dwButtonState != 0 && internal_IO_WaitingForMousePress) {
                    internal_IO_WaitingForMousePress = false;
                    break;
                }
                if(mouseClickHandler != NULL) {
                    mouseClickHandler((int)mer.dwButtonState, mouseHandlerData);
                }
                break;
            case DOUBLE_CLICK:
                if(mouseDoubleClickHandler != NULL) {
                    mouseDoubleClickHandler((int)mer.dwButtonState, mouseHandlerData);
                }
                break;
            case MOUSE_WHEELED:
                if(mouseScrollHandler != NULL) {
                    mouseScrollHandler((mer.dwButtonState & 0x80000000) != 0, mouseHandlerData);
                }
                break;
            case MOUSE_MOVED:
                if(mouseMoveHandler != NULL) {
                    mouseMoveHandler(mouseHandlerData);
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

void Mouse_IOLoop()
{
    if(internal_IOHelper_LoopLock) return;

    if(merQueueReadPtr != merQueueWritePtr) {
        MouseEventProc(merQueue[merQueueReadPtr]);
        MerBufferPtrInc(merQueueReadPtr);
    }
}
