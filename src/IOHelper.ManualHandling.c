#include "IOHelper.h"
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include "AnsiDefinitions.h"
#include "DebugCheck.h"
#include "Settings.h"

#include "IOHelper.ManualHandling.Mouse.h"
#include "IOHelper.ManualHandling.Window.h"
#include "IOHelper.ManualHandling.Keyboard.h"

#ifdef PROGRAMISTERZY_EXTENDED_TERMINAL_INTEGRATION

bool IOHelper_LoopLock = false;
bool internal_IOHelper_LoopLock = false;

HANDLE stdinHandle;
HANDLE stdoutHandle;
DWORD fdwStdInOldMode;
DWORD fdwStdOutOldMode;

DWORD fdwInMode = ENABLE_WINDOW_INPUT;
DWORD fdwOutMode = 0;

bool stdInHandleInitialized = false;
bool stdOutHandleInitialized = false;

void ErrorExit(LPSTR);

void SetConsoleModes() {
    // Enable the window and mouse input events.
    if (!SetConsoleMode(stdinHandle, fdwInMode)) {
        ErrorExit("SetConsoleMode(STDIN)");
    }

    if(fdwOutMode != 0) {
        // Set output mode to handle virtual terminal sequences
        if (!SetConsoleMode(stdinHandle, fdwInMode)) {
            ErrorExit("SetConsoleMode(STDOUT)");
        }
    }
}

void SetThisConsoleTitle(const char *title)
{
    SetConsoleTitle(title);
}

extern Settings* LoadedSettings;

void EnableMouseInput(bool enable)
{
    if(enable && !LoadedSettings->EnableMouseSupport) {
        return;
    }

    if(enable == ((fdwInMode & ENABLE_MOUSE_INPUT) != 0)) return;

    if(enable) {
        fdwInMode |= ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS;
    }
    else {
        fdwInMode &= ~((DWORD)(ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS));
    }

    SetConsoleModes();
}

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

    SetConsoleModes();
    
    if(!CheckForAnsiSupport()) {
        fprintf(stderr, "ANSI not supported but requied!\n");
        ExitApp(EXIT_FAILURE);
    }

    EnableAlternativeBuffer();
}

void ExitApp(int exitCode)
{
    // Restore input mode on exit.
    if(stdInHandleInitialized) SetConsoleMode(stdinHandle, fdwStdInOldMode);

    // Restore output mode on exit.
    if(stdOutHandleInitialized) SetConsoleMode(stdoutHandle, fdwStdOutOldMode);

    DisableAlternativeBuffer();

    exit(exitCode);
}

void ErrorExit(LPSTR lpszMessage)
{
    fprintf(stderr, "[IO] %s\n", lpszMessage);
    ExitApp(EXIT_FAILURE);
}

bool internal_IO_WaitingForMousePress = false;
int WaitForAnyInput() {
    internal_IO_WaitingForMousePress = true;
    while(true) {

        if(!internal_IO_WaitingForMousePress) {
            return INT_MAX;
        }

        if(kbhit()) {
            return getch();
        }

        IOLoop();
    }
}

#define INPUT_RECORD_BUFFER_SIZE (DWORD)(128)
INPUT_RECORD irInBuf[INPUT_RECORD_BUFFER_SIZE];
DWORD cNumRead;

// Based on https://docs.microsoft.com/en-us/windows/console/reading-input-buffer-events
void IOLoop()
{
    Window_IOLoop();
    Mouse_IOLoop();

    if(!GetNumberOfConsoleInputEvents(stdinHandle, &cNumRead)) {
        ErrorExit("GetNumberOfConsoleInputEvents");
    }

    if(cNumRead == 0) return; // No events to process

    if (!ReadConsoleInput(
            stdinHandle,                // input buffer handle
            irInBuf,                    // buffer to read into
            INPUT_RECORD_BUFFER_SIZE,   // size of read buffer
            &cNumRead))                 // number of records read
    {
        ErrorExit("ReadConsoleInput");
    }

    // We want to handle only the last window resize event
    DWORD lastWindowResizeEventId = 0;
    for (DWORD i = 0; i < cNumRead; i++)
    {
        if(irInBuf[i].EventType == WINDOW_BUFFER_SIZE_EVENT) {
            lastWindowResizeEventId = i;
        }
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
                MouseEventProc(irInBuf[i].Event.MouseEvent);
                break;

            case WINDOW_BUFFER_SIZE_EVENT: // scrn buf. resizing
                if(i != lastWindowResizeEventId) continue; // Skip all but the last resize event

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

#endif
