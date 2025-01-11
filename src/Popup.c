#include "Popup.h"
#include "Utf8Symbols.h"
#include "AnsiHelper.h"
#include "IOHelper.h"
#include "TextHelper.h"

extern int LatestTerminalWidth, LatestTerminalHeight;

void DrawEmptyPopup(int width, int height, int* outBeginX, int* outBeginY)
{
    const int beginX = (LatestTerminalWidth - width) / 2;
    const int beginY = (LatestTerminalHeight - height) / 2;

    SetCursorPosition(beginX, beginY);
    PRINT_SINGLE_TOP_BORDER(width);

    for (int i = 1; i < height - 1; i++)
        PrintGenericBorderEdges(beginX, width, beginY + i, SINGLE_VERTICAL_LINE, true);

    SetCursorPosition(beginX, beginY + height - 1);
    PRINT_SINGLE_BOTTOM_BORDER(width);

    *outBeginX = beginX;
    *outBeginY = beginY;
}

void _internal_ShowAlertPopupKeys(const char *message, int popUpWidth, int keyCount, char *keys)
{
    int popUpHeight = 2 + GetWrappedLineCount(message, popUpWidth - 4) + 2;

    int beginX, beginY;
    DrawEmptyPopup(popUpWidth, popUpHeight, &beginX, &beginY);

    SetCursorPosition(beginX + 2, beginY + 2);
    PrintWrappedLine(message, popUpWidth - 4, beginX + 2, true);

    _internal_WaitForKeys(keyCount, keys);
}

void ShowAlertPopup(const char* message, int popUpWidth)
{
    _internal_ShowAlertPopupKeys(message, popUpWidth, 2, (char[]){ENTER, ESC});
}

bool ShowConfirmationPopup(const char* message, const char* confirmText, const char* cancelText, int popUpWidth)
{
    int confirmTextWidth = GetStringCharCount(confirmText) + 8;
    int cancelTextWidth = GetStringCharCount(cancelText) + 6;

    bool split = confirmTextWidth + cancelTextWidth + 4 + 1 + 2 > popUpWidth;

    int popUpHeight = 2 + GetWrappedLineCount(message, popUpWidth - 4) + 2 + (split ? 1 : 0);

    int beginX, beginY;
    DrawEmptyPopup(popUpWidth, popUpHeight, &beginX, &beginY);

    SetCursorPosition(beginX + 2, beginY + 1);
    PrintWrappedLine(message, popUpWidth - 4, beginX + 2, true);
    SetCursorPosition(beginX + 2, beginY + popUpHeight - 2);
    printf("[ENTER] %s", confirmText);
    if(split) printf(CSR_MOVE_DOWN_1);
    printf("\r" CSR_MOVE_RIGHT(beginX + 2 + popUpWidth - 4 - cancelTextWidth - 1));
    printf("%s [ESC]", cancelText);

    char c = WaitForKeys(ENTER, ESC);

    return c == ENTER;
}
