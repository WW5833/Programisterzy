#ifndef _INC_ANSIHELPER_H
#define _INC_ANSIHELPER_H

#include "AnsiDefinitions.h"

void GetCursorPosition(int* x, int* y);
void SetCursorPosition(int x, int y);
void ClearScreen();
void ClearLine();
void SetColors(int fgColor, int bgColor);
void SetColor(int color);
void ResetColor();
void ResetCursor();
void HideCursor();
void ShowCursor();

#endif // _INC_ANSIHELPER_H
