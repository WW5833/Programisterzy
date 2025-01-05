#include "IOHelper.h"
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include "AnsiDefinitions.h"
#include "DebugCheck.h"

bool CheckForAnsiSupport() {
    printf(ESC_SEQ "6n");
    if(_kbhit() && _getch() == '\x1B') {
        while (_getch() != 'R'); // Clear the buffer
        printf(ESC_SEQ "0;0H" CLR_SCRN ESC_SEQ "0;0H");
        return true; // ANSI supported
    }

    return false;
}

#ifndef PROGRAMISTERZY_EXTENDED_TERMINAL_INTEGRATION
    int getch()
    {
        int tor = _getch();
        if(tor == CTRL_C) {
            ExitApp(EXIT_SUCCESS);
        }

        return tor;
    }

    int kbhit()
    {
        return _kbhit();
    }

    void InitializeIO()
    {
        system("chcp 65001");

        if(CheckForAnsiSupport()) return;

        // Attempt to enable ANSI support
        HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
        SetConsoleMode(hInput, ENABLE_VIRTUAL_TERMINAL_INPUT);
        HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleMode(hOutput, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

        if(CheckForAnsiSupport()) return;

        fprintf(stderr, "ANSI not supported but requied!\n");
        ExitApp(EXIT_FAILURE);
    }

    void ExitApp(int exitCode)
    {
        exit(exitCode);
    }

    void SetResizeHandler(void (*handler)(int, int, void *), void *data)
    {
    }

    void UnsetResizeHandler()
    {
    }

    void IOLoop()
    {
        // Do nothing
    }
#else

#define MAX_Y_SIZE_CONSOLE 150

#define TEXT_INPUT_BUFFER_SIZE 256
#define CTRL_C '\03'

int textInputBufferReadPtr = 0;
int textInputBufferWritePtr = 0;
char textInputBuffer[TEXT_INPUT_BUFFER_SIZE];

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

INPUT_RECORD irInBuf[128];
DWORD cNumRead;

HANDLE stdinHandle;
HANDLE stdoutHandle;
DWORD fdwStdInOldMode;
DWORD fdwStdOutOldMode;

DWORD fdwInMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
DWORD fdwOutMode = 0;

bool stdInHandleInitialized = false;
bool stdOutHandleInitialized = false;

void ErrorExit(LPSTR);
void KeyEventProc(KEY_EVENT_RECORD);
void MouseEventProc(MOUSE_EVENT_RECORD);
void ResizeEventProc(WINDOW_BUFFER_SIZE_RECORD);
void CallResizeHandler(int width, int height);

int latestTerminalWidth = -1;
int latestTerminalHeight = -1;

// Based on https://docs.microsoft.com/en-us/windows/console/reading-input-buffer-events
void InitializeIO()
{
    system("chcp 65001");

    // Get the standard input handle.
    stdinHandle = GetStdHandle(STD_INPUT_HANDLE);
    if (stdinHandle == INVALID_HANDLE_VALUE) {
        ErrorExit("GetStdHandle(STDIN)");
    }

    // Save the current input mode, to be restored on exit.
    if (! GetConsoleMode(stdinHandle, &fdwStdInOldMode)){
        ErrorExit("GetConsoleMode(STDIN)");
    }

    stdInHandleInitialized = true;

    // Get the standard output handle.
    stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    if (stdoutHandle == INVALID_HANDLE_VALUE) {
        ErrorExit("GetStdHandle(STDOUT)");
    }

    // Save the current output mode, to be restored on exit.
    if (! GetConsoleMode(stdoutHandle, &fdwStdOutOldMode)) {
        ErrorExit("GetConsoleMode(STDOUT)");
    }

    stdOutHandleInitialized = true;

    if(!CheckForAnsiSupport()) {
        fdwInMode |= ENABLE_VIRTUAL_TERMINAL_INPUT;
        fdwOutMode |= ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    }

    // Enable the window and mouse input events.
    if (! SetConsoleMode(stdinHandle, fdwInMode)) {
        ErrorExit("SetConsoleMode(STDIN)");
    }

    if(fdwOutMode != 0) {
        // Set output mode to handle virtual terminal sequences
        if (! SetConsoleMode(stdinHandle, fdwInMode)) {
            ErrorExit("SetConsoleMode(STDOUT)");
        }
    }
    
    if(!CheckForAnsiSupport()) {
        fprintf(stderr, "ANSI not supported but requied!\n");
        ExitApp(EXIT_FAILURE);
    }
}

void ExitApp(int exitCode)
{
    // Restore input mode on exit.
    if(stdInHandleInitialized) SetConsoleMode(stdinHandle, fdwStdInOldMode);

    // Restore output mode on exit.
    if(stdOutHandleInitialized) SetConsoleMode(stdoutHandle, fdwStdOutOldMode);

    exit(exitCode);
}

void ErrorExit(LPSTR lpszMessage)
{
    fprintf(stderr, "[IO] %s\n", lpszMessage);
    ExitApp(EXIT_FAILURE);
}


// Based on https://docs.microsoft.com/en-us/windows/console/reading-input-buffer-events
void IOLoop()
{
    CallResizeHandler(latestTerminalWidth, latestTerminalHeight);

    if(!GetNumberOfConsoleInputEvents(stdinHandle, &cNumRead)) {
        ErrorExit("GetNumberOfConsoleInputEvents");
    }

    if(cNumRead == 0) return; // No events to process

    if (!ReadConsoleInput(
            stdinHandle,      // input buffer handle
            irInBuf,     // buffer to read into
            128,         // size of read buffer
            &cNumRead)) // number of records read
    {
        ErrorExit("ReadConsoleInput");
    }

    // Dispatch the events to the appropriate handler.
    for (DWORD i = 0; i < cNumRead; i++)
    {
        switch(irInBuf[i].EventType)
        {
            case KEY_EVENT: // keyboard input
                KeyEventProc(irInBuf[i].Event.KeyEvent);
                break;

            case MOUSE_EVENT: // mouse input
                // MouseEventProc(irInBuf[i].Event.MouseEvent);
                break;

            case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing
                ResizeEventProc(irInBuf[i].Event.WindowBufferSizeEvent);
                break;

            case FOCUS_EVENT:   // disregard focus events
            case MENU_EVENT:    // disregard menu events
                break;

            default:
                ErrorExit("Unknown event type");
                break;
        }
    }
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


void MouseEventProc(MOUSE_EVENT_RECORD mer)
{
    switch(mer.dwEventFlags)
    {
        case 0:
            printf("Mouse event: ");
            if(mer.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
            {
                printf("left button press \n");
            }
            else if(mer.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
            {
                printf("right button press \n");
            }
            else
            {
                printf("button press\n");
            }
            break;
        case DOUBLE_CLICK:
            printf("Mouse event: ");
            printf("double click\n");
            break;
        case MOUSE_WHEELED:
            printf("Mouse event: ");
            printf("vertical mouse wheel\n");
            break;
    }
}


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

time_t lastResizeCall = 0;
bool resizeCallPending = false;
void CallResizeHandler(int width, int height)
{
    if(resizeHandler == NULL || !resizeCallPending) return;

    if(difftime(time(NULL), lastResizeCall) < 0.25) {
        return;
    }

    char buffer[256];

    sprintf(buffer, "Programisterzy [%d x %d]", width, height);

    SetConsoleTitle(buffer);

    lastResizeCall = time(NULL);
    resizeCallPending = false;
    resizeHandler(width, height, resizeHandlerData);
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

#endif
