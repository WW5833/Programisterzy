#include "Utf8Symbols.h"
#include <stdio.h>
#include "AnsiHelper.h"

void PrintGenericBorder(
    int width, 
    const char* leftFormat,
    const char* middleFormat,
    const char* rightFormat) 
{
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    printf("%s", leftFormat);
    for (int _i = 0; _i < width - 2; _i++)
        printf("%s", middleFormat);
    printf("%s", rightFormat);
    printf(CSR_MOVE_LEFT_0_DOWN1);
#pragma GCC diagnostic warning "-Wformat-nonliteral"
}

void PrintGenericBorderEdges(
    int startX, int width, int y,
    const char* format,
    bool clearInside) 
{
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    SetCursorPosition(startX, y);
    printf("%s", format);
    if (clearInside)
        for (int j = 0; j < width - 2; j++) printf(" ");
    else
        SetCursorPosition(startX + width, y);
    printf("%s", format);
#pragma GCC diagnostic warning "-Wformat-nonliteral"
}
