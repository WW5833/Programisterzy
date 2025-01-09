#ifndef _INC_POPUP_H
#define _INC_POPUP_H

#include <stdbool.h>

void DrawEmptyPopup(int width, int height, int* outBeginX, int* outBeginY);

void ShowAlertPopup(const char* message, int popUpWidth);
void _internal_ShowAlertPopupKeys(const char* message, int popUpWidth, int keyCount, char* keys);
#define ShowAlertPopupKeys(message, popUpWidth, keys) _internal_ShowAlertPopupKeys(message, popUpWidth, sizeof(keys) / sizeof(char), keys)

bool ShowConfirmationPopup(const char* message, const char* confirmText, const char* cancelText, int popUpWidth);

#endif // _INC_POPUP_H