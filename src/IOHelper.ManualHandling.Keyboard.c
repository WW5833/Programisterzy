#include "IOHelper.ManualHandling.Keyboard.h"
#include <stdio.h>
#include "DebugCheck.h"

#ifdef PROGRAMISTERZY_EXTENDED_TERMINAL_INTEGRATION

#define TEXT_INPUT_BUFFER_SIZE 256
#define CTRL_C '\03'

int textInputBufferReadPtr = 0;
int textInputBufferWritePtr = 0;
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
    if(!(IOHelper_LoopLock || internal_IOHelper_LoopLock)) IOLoop();
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

#endif
