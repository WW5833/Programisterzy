#include "Utf8Symbols.h"
#include <stdio.h>
#include "AnsiHelper.h"

void PrintGenericBorder(
    int width,
    const char* leftCharacter,
    const char* middleCharacter,
    const char* rightCharacter)
{
    printf("%s", leftCharacter);
    for (int _i = 0; _i < width - 2; _i++)
        printf("%s", middleCharacter);
    printf("%s", rightCharacter);
    printf(CSR_MOVE_LEFT_0_DOWN1);
}

void PrintGenericBorderEdges(
    int startX, int width, int y,
    const char* character,
    bool clearInside)
{
    SetCursorPosition(startX, y);
    printf("%s", character);
    if (clearInside)
        for (int j = 0; j < width - 2; j++) printf(" ");
    else
        SetCursorPosition(startX + width, y);
    printf("%s", character);
}

void PrintGenericBorderEdgesHere(
    int startX, int width,
    const char* character)
{
    printf("\r");
    if(startX > 0) printf(CSR_MOVE_RIGHT(startX));
    printf("%s", character);
    printf(CSR_MOVE_RIGHT(width - 1));
    printf("%s", character);
}
