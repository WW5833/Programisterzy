#ifndef _INC_POPUP_H
#define _INC_POPUP_H

#include <stdbool.h>

void DrawEmptyPopup(int width, int height, int* outBeginX, int* outBeginY);

void ShowAlertPopup(const char* message, int popUpWidth);
void ShowAlertPopupWithTitle(const char* title, const char* message, int popUpWidth);
void _internal_ShowAlertPopupWithTitleKeys(const char* title, const char* message, int popUpWidth, int keyCount, char *keys);
#define ShowAlertPopupKeys(message, popUpWidth, ...) _internal_ShowAlertPopupWithTitleKeys(NULL, message, popUpWidth, sizeof((char[]){__VA_ARGS__}), (char[]){__VA_ARGS__})
#define ShowAlertPopupWithTitleKeys(title, message, popUpWidth, ...) _internal_ShowAlertPopupWithTitleKeys(title, message, popUpWidth, sizeof((char[]){__VA_ARGS__}), (char[]){__VA_ARGS__})

bool ShowConfirmationPopup(const char* message, const char* confirmText, const char* cancelText, int popUpWidth);

#endif // _INC_POPUP_H
