#include "IOHelper.Keyboard.h"
#include <stdio.h>
#include "IOHelper.Window.h"

#define TEXT_INPUT_BUFFER_SIZE 1024
#define CTRL_C '\03'

static int textInputBufferReadPtr = TEXT_INPUT_BUFFER_SIZE;
static int textInputBufferWritePtr = TEXT_INPUT_BUFFER_SIZE;
static char textInputBuffer[TEXT_INPUT_BUFFER_SIZE];

#define BufferPtrInc(ptr) ptr++; if(ptr >= TEXT_INPUT_BUFFER_SIZE) ptr = 0;

int getch()
{
    while(textInputBufferReadPtr == textInputBufferWritePtr)
        IOLoop();

    BufferPtrInc(textInputBufferReadPtr);

    return textInputBuffer[textInputBufferReadPtr];
}

int kbhit()
{
    IOLoop();
    return textInputBufferReadPtr != textInputBufferWritePtr;
}

/// @brief If true, treat Shift + R as a resize event
bool TreatShiftRAsResizeEvent = false;

void KeyEventProc(KEY_EVENT_RECORD ker)
{
    if(ker.bKeyDown)
    {
        if(ker.uChar.AsciiChar == CTRL_C) {
            ExitApp(EXIT_SUCCESS);
        }

        if(TreatShiftRAsResizeEvent && ker.uChar.AsciiChar == 'R') {
            EnqueuResizeEventCall();
            return;
        }

        if(ker.uChar.AsciiChar == '\t' && ker.dwControlKeyState & SHIFT_PRESSED) {
            BufferPtrInc(textInputBufferWritePtr);
            textInputBuffer[textInputBufferWritePtr] = SHIFT_TAB; // Use VT (Vertical Tab) as a Shift + Tab
            return;
        }

        if(ker.uChar.AsciiChar != 0) {
            BufferPtrInc(textInputBufferWritePtr);
            textInputBuffer[textInputBufferWritePtr] = ker.uChar.AsciiChar;
            return;
        }

        if((ker.dwControlKeyState & ENHANCED_KEY) != 0) {
            if(ker.wVirtualKeyCode < VK_END) return; // Ignore not supported keys
            if(ker.wVirtualKeyCode > VK_DELETE) return; // Ignore not supported keys
            BufferPtrInc(textInputBufferWritePtr);
            textInputBuffer[textInputBufferWritePtr] = ESCAPE_CHAR;
            BufferPtrInc(textInputBufferWritePtr);
            textInputBuffer[textInputBufferWritePtr] = (char)ker.wVirtualKeyCode;
            if((ker.dwControlKeyState & SHIFT_PRESSED) != 0) {
                textInputBuffer[textInputBufferWritePtr] |= VK_SHIFT_MOD;
            }
            return;
        }
    }
}
