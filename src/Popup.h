#ifndef _INC_POPUP_H
#define _INC_POPUP_H

#include <stdbool.h>

/// @brief Draw a popup with the size
/// @param width The width of the popup
/// @param height The height of the popup
/// @param outBeginX The output popup top-left X position
/// @param outBeginY The output popup top-left Y position
void DrawEmptyPopup(int width, int height, int* outBeginX, int* outBeginY);

/// @brief Draw a popup with the width and message
/// @param title The message of the popup
/// @param width The width of the popup
void ShowAlertPopup(const char* message, int popUpWidth);
/// @brief Draw a popup with the title and message
/// @param title The title of the popup
/// @param message The message of the popup
/// @param width The width of the popup
void ShowAlertPopupWithTitle(const char* title, const char* message, int popUpWidth);
/// @brief Draw a popup with the title and message
/// @param title The title of the popup
/// @param message The message of the popup
/// @param width The width of the popup
/// @param keyCount The number of keys to wait for
/// @param keys The keys to wait for
void _internal_ShowAlertPopupWithTitleKeys(const char* title, const char* message, int popUpWidth, int keyCount, char *keys);
/// @brief Draw a popup with the title and message
/// @param message The message of the popup
/// @param width The width of the popup
/// @param ... The keys to wait for
#define ShowAlertPopupKeys(message, popUpWidth, ...) _internal_ShowAlertPopupWithTitleKeys(NULL, message, popUpWidth, sizeof((char[]){__VA_ARGS__}), (char[]){__VA_ARGS__})

/// @brief Draw a popup with the title and message
/// @param title The title of the popup
/// @param message The message of the popup
/// @param width The width of the popup
/// @param ... The keys to wait for
#define ShowAlertPopupWithTitleKeys(title, message, popUpWidth, ...) _internal_ShowAlertPopupWithTitleKeys(title, message, popUpWidth, sizeof((char[]){__VA_ARGS__}), (char[]){__VA_ARGS__})

/// @brief Show a confirmation popup
/// @param message The message of the popup
/// @param confirmText The text for the confirm button
/// @param cancelText The text for the cancel button
/// @param popUpWidth The width of the popup
/// @return true if confirmed, false if canceled
bool ShowConfirmationPopup(const char* message, const char* confirmText, const char* cancelText, int popUpWidth);

#endif // _INC_POPUP_H
