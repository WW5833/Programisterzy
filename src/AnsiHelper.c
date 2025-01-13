#include "AnsiHelper.h"
#include <stdio.h>
#include <stdlib.h>
#include "IOHelper.h"
#include <conio.h>
#include "Settings.h"
#include "RGBColors.h"

#define BUFFER_SIZE 128
char _tmp_buffer[BUFFER_SIZE];

extern Settings* LoadedSettings;

void GetCursorPosition(int* x, int* y)
{
    while (kbhit()) getch(); // Clear input buffer

    printf(ESC_SEQ "6n");

    int c = getch();

    if(c != ESC_SEQ_CHAR) {
        while(kbhit()) {
            c = getch();
            if(c == ESC_SEQ_CHAR){
                goto esc_seq_found;
            }
        }

        ExitAppWithErrorMessage(EXIT_FAILURE, ERRMSG_ANSI_CURSOR_FAILED);
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

bool CheckForAnsiSupport() {
    printf(ESC_SEQ "6n");

    // Use _getch() instead of custom getch() because this function is called before IOHelper is fully initialized
    if(_kbhit() && _getch() == ESC_SEQ_CHAR) {
        while (_getch() != 'R'); // Clear the buffer
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

extern int LatestTerminalWidth, LatestTerminalHeight;
void ClearScreenManual()
{
    for (int i = 1; i <= LatestTerminalWidth; i++)
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

void ResetColor()
{
    SetResetColor();
}

void ResetCursor()
{
    SetCursorPosition(0, 0);
}

void SaveCursorPosition()
{
    printf(ESC_SEQ "s");
}

void RestoreCursorPosition()
{
    printf(ESC_SEQ "u");
}

void HideConsoleCursor()
{
    printf(ESC_SEQ "?25l");
}

void ShowConsoleCursor()
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
