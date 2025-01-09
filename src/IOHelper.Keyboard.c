#include "IOHelper.Keyboard.h"
#include <stdio.h>

#define TEXT_INPUT_BUFFER_SIZE 128
#define CTRL_C '\03'

int textInputBufferReadPtr = TEXT_INPUT_BUFFER_SIZE;
int textInputBufferWritePtr = TEXT_INPUT_BUFFER_SIZE;
char textInputBuffer[TEXT_INPUT_BUFFER_SIZE];

#define BufferPtrInc(ptr) ptr++; if(ptr >= TEXT_INPUT_BUFFER_SIZE) ptr = 0;

extern bool IOHelper_LoopLock;
extern bool internal_IOHelper_LoopLock;

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

void KeyEventProc(KEY_EVENT_RECORD ker)
{
    if(ker.bKeyDown)
    {
        if(ker.uChar.AsciiChar == CTRL_C) {
            ExitApp(EXIT_SUCCESS);
        }

        if(ker.uChar.AsciiChar != 0) {
            BufferPtrInc(textInputBufferWritePtr);
            textInputBuffer[textInputBufferWritePtr] = ker.uChar.AsciiChar;
            return;
        }

        if((ker.dwControlKeyState & ENHANCED_KEY) != 0) {
            if((ker.wVirtualScanCode & 0x40) == 0) return; // Ignore non arrows
            BufferPtrInc(textInputBufferWritePtr);
            textInputBuffer[textInputBufferWritePtr] = ESCAPE_CHAR;
            BufferPtrInc(textInputBufferWritePtr);
            textInputBuffer[textInputBufferWritePtr] = (char)ker.wVirtualScanCode;
            return;
        }
    }
}
