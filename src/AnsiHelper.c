#include "AnsiHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "PageUtils.h"

#define SET_COLOR(x) ESC_SEQ "%dm", x
#define CSR_MOVE_TO(x, y) ESC_SEQ "%d;%dH", x, y
#define CLR_LINE ESC_SEQ "2K" // Clear line
#define CLR_SCRN ESC_SEQ "2J" // Clear screen

#define BUFFER_SIZE 128
char _tmp_buffer[BUFFER_SIZE];

void GetCursorPosition(int* x, int* y)
{
    while (_kbhit()) _getch(); // Clear input buffer
    
    printf(ESC_SEQ "6n");

    int c = _getch();
    if(c == CTRL_C) {
        ExitOnCtrlC();
    }
    
    if(c != '\x1B') {
        while(_kbhit()) {
            c = _getch();
            if(c == CTRL_C) {
                ExitOnCtrlC();
            }
            if(c == '\x1B'){
                goto esc_seq_found;
            }
        }

        fprintf(stderr, "Failed to get cursor position, ANSI not supported?\n");
        exit(EXIT_FAILURE);
    }
    esc_seq_found:

    c = _getch(); // Shold be '['
    int i = 0;
    while(c != ';' && c != '\0') {
        c = _getch();
        _tmp_buffer[i++] = (char)c;
    }
    _tmp_buffer[i] = '\0';
    *y = atoi(_tmp_buffer);

    i = 0;
    while(c != 'R' && c != '\0') {
        c = _getch();
        _tmp_buffer[i++] = (char)c;
    }
    _tmp_buffer[i] = '\0';
    *x = atoi(_tmp_buffer);
}

void SetCursorPosition(int x, int y)
{
    printf(ESC_SEQ "%d;%dH", y, x);
}

void GetTerminalSize(int* x, int* y)
{
    SetCursorPosition(999, 999);
    GetCursorPosition(x, y);
}

void ClearScreen()
{
    printf(CLR_SCRN);
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
