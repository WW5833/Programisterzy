#ifndef _INC_ANSIHELPER_H
#define _INC_ANSIHELPER_H

#include "AnsiDefinitions.h"

void ClearScreen();
void ClearLine();
void ResetColor();
void ResetCursor();
void HideCursor();
void ShowCursor();
void SaveCursorPosition();
void RestoreCursorPosition();

#endif // _INC_ANSIHELPER_H
