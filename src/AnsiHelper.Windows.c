#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>

#include "AnsiHelper.Windows.h"

#define ESC_SEQ "\x1B["
#define CLR_SCRN ESC_SEQ "2J" // Clear screen

void EnsureAnsiSupport()
{
    printf(ESC_SEQ "6n");
    if(_kbhit() && getch() == '\x1B') {
        while (getch() != 'R'); // Clear the buffer
        printf(CLR_SCRN);
        return; // ANSI supported
    }

    HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
    SetConsoleMode(hInput, ENABLE_VIRTUAL_TERMINAL_INPUT);
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleMode(hOutput, ENABLE_PROCESSED_OUTPUT | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

    printf(ESC_SEQ "6n");
    if(_kbhit() && getch() == '\x1B') {
        while (getch() != 'R'); // Clear the buffer
        printf(CLR_SCRN);
        return; // ANSI now supported
    }
    
    fprintf(stderr, "ANSI not supported but is requied!\n");
    exit(EXIT_FAILURE);
}