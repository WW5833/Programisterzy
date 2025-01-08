#include "AnsiHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include "IOHelper.h"
#include "PageUtils.h"
#include "AnsiDefinitions.h"
#include <conio.h>

#define SET_COLOR(x) ESC_SEQ "%dm", x
#define CSR_MOVE_TO(x, y) ESC_SEQ "%d;%dH", x, y
#define CLR_LINE ESC_SEQ "2K" // Clear line

#define BUFFER_SIZE 128
char _tmp_buffer[BUFFER_SIZE];

extern bool IOHelper_LoopLock;

void GetCursorPosition(int* x, int* y)
{
    while (kbhit()) getch(); // Clear input buffer
    
    printf(ESC_SEQ "6n");

    int c = getch();
    if(c == CTRL_C) {
        ExitOnCtrlC();
    }
    
    if(c != '\x1B') {
        while(kbhit()) {
            c = getch();
            if(c == CTRL_C) {
                ExitOnCtrlC();
            }
            if(c == '\x1B'){
                goto esc_seq_found;
            }
        }

        fprintf(stderr, "Failed to get cursor position, ANSI not supported?\n");
        ExitApp(EXIT_FAILURE);
    }
    esc_seq_found:

    c = getch(); // Shold be '['
    int i = 0;
    while(c != ';' && c != '\0') { // Read y position
        c = getch();
        _tmp_buffer[i++] = (char)c;
    }
    _tmp_buffer[i] = '\0';
    *y = atoi(_tmp_buffer);

    i = 0;
    while(c != 'R' && c != '\0') { // Read x position
        c = getch();
        _tmp_buffer[i++] = (char)c;
    }
    _tmp_buffer[i] = '\0';
    *x = atoi(_tmp_buffer);
}

void SetCursorPosition(int x, int y)
{
    printf(ESC_SEQ "%d;%dH", y, x);
}

void GetTerminalSize(int *x, int *y)
{
    IOHelper_LoopLock = true;
    SetCursorPosition(999, 999);
    GetCursorPosition(x, y);
    IOHelper_LoopLock = false;
}

bool CheckForAnsiSupport() {
    printf(ESC_SEQ "6n");
    
    // Use _getch() instead of custom getch() because this function is called before IOHelper is fully initialized
    if(_kbhit() && _getch() == '\x1B') { 
        while (_getch() != 'R'); // Clear the buffer
        ClearScreenManual();
        return true; // ANSI supported
    }

    return false;
}

void ClearScreen()
{
    ResetCursor();
    printf(CLR_SCRN);
    ResetCursor();
}

void ClearScreenManual() 
{
    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);
    for (int i = 1; i <= terminalHeight; i++)
    {
        SetCursorPosition(0, i);
        ClearLine();
    }
    ResetCursor();    
}

void ClearLine()
{
    printf(CLR_LINE);
}

void SetColors(int fgColor, int bgColor)
{
    printf(SET_COLOR(fgColor));
    printf(SET_COLOR(bgColor));
}

void SetColor(int color)
{
    printf(SET_COLOR(color));
}

void ResetColor()
{
    // printf(SET_COLOR(COLOR_RESET));
    SetColors(COLOR_FG_DEFAULT, COLOR_BG_BLACK);
}

void ResetCursor()
{
    SetCursorPosition(0, 0);
}

void HideCursor()
{
    printf(ESC_SEQ "?25l");
}

void ShowCursor()
{
    printf(ESC_SEQ "?25h");
}

void EnableAlternativeBuffer()
{
    printf(SCREEN_ALTERNATIVE_BUFFER_ENABLE);
}

void DisableAlternativeBuffer()
{
    printf(SCREEN_ALTERNATIVE_BUFFER_DISABLE);
}
