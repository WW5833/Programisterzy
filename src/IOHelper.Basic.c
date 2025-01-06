#include "IOHelper.h"
#include <conio.h>
#include <windows.h>
#include <stdio.h>
#include "AnsiDefinitions.h"
#include "DebugCheck.h"

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
#endif
