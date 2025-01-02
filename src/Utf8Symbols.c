#include "Utf8Symbols.h"
#include <stdio.h>
#include "AnsiHelper.h"

void PrintGenericBorder(
    int width, 
    const char* leftFormat, 
    const int leftArg1, 
    const int leftArg2, 
    const int leftArg3,
    const char* middleFormat, 
    const int middleArg1, 
    const int middleArg2, 
    const int middleArg3,
    const char* rightFormat, 
    const int rightArg1, 
    const int rightArg2, 
    const int rightArg3) 
{
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    printf(leftFormat, leftArg1, leftArg2, leftArg3);
    for (int _i = 0; _i < width - 2; _i++)
        printf(middleFormat, middleArg1, middleArg2, middleArg3);
    printf(rightFormat, rightArg1, rightArg2, rightArg3);
    printf("\n");
#pragma GCC diagnostic warning "-Wformat-nonliteral"
}

void PrintGenericBorderEdges(
    int startX, int width, int y,
    const char* format, 
    const int arg1, 
    const int arg2, 
    const int arg3,
    bool clearInside) 
{
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
    SetCursorPosition(startX, y);
    printf(format, arg1, arg2, arg3);
    if (clearInside)
        for (int j = 0; j < width - 2; j++) printf(" ");
    else
        SetCursorPosition(startX + width, y);
    printf(format, arg1, arg2, arg3);
#pragma GCC diagnostic warning "-Wformat-nonliteral"
}
