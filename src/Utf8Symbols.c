#include "Utf8Symbols.h"
#include <stdio.h>
#include "AnsiHelper.h"

void PrintGenericBorder(
    int width,
    const char* leftFormat,
    const char* middleFormat,
    const char* rightFormat)
{
    printf("%s", leftFormat);
    for (int _i = 0; _i < width - 2; _i++)
        printf("%s", middleFormat);
    printf("%s", rightFormat);
    printf(CSR_MOVE_LEFT_0_DOWN1);
}

void PrintGenericBorderEdges(
    int startX, int width, int y,
    const char* format,
    bool clearInside)
{
    SetCursorPosition(startX, y);
    printf("%s", format);
    if (clearInside)
        for (int j = 0; j < width - 2; j++) printf(" ");
    else
        SetCursorPosition(startX + width, y);
    printf("%s", format);
}

void PrintGenericBorderEdgesHere(
    int startX, int width,
    const char* format)
{
    printf("\r");
    if(startX > 0) printf(CSR_MOVE_RIGHT(startX));
    printf("%s", format);
    printf(CSR_MOVE_RIGHT(width - 1));
    printf("%s", format);
}
