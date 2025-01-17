#ifndef _INC_ANSIHELPER_H
#define _INC_ANSIHELPER_H

#include <stdbool.h>

#define ESC_SEQ_CHAR '\x1B'
#define ESC_SEQ "\x1B["

#define CLR_LINE_END ESC_SEQ "0K"   // Clear from cursor to end of line
#define CLR_LINE_START ESC_SEQ "1K" // Clear from cursor to start of line
#define CLR_LINE ESC_SEQ "2K"       // Clear line

#define CLR_SCRN ESC_SEQ "2J"       // Clear screen

#define CSR_MOVE_TO(x, y) ESC_SEQ "%d;%dH", x, y

#define CSR_MOVE_UP(x) ESC_SEQ "%dA", x
#define CSR_MOVE_DOWN(x) ESC_SEQ "%dB", x
#define CSR_MOVE_RIGHT(x) ESC_SEQ "%dC", x
#define CSR_MOVE_LEFT(x) ESC_SEQ "%dD", x

#define CSR_MOVE_LEFT_0_DOWN(x) ESC_SEQ "%dE", x

#define CSR_MOVE_CURSOR_X(x) ESC_SEQ "%dG", x

#define CSR_MOVE_UP_1 ESC_SEQ "1A"
#define CSR_MOVE_DOWN_1 ESC_SEQ "1B"
#define CSR_MOVE_RIGHT_1 ESC_SEQ "1C"
#define CSR_MOVE_LEFT_1 ESC_SEQ "1D"
#define CSR_MOVE_LEFT_0_DOWN1 ESC_SEQ "1E"

#define UNDERLINE_ON ESC_SEQ "4m"
#define UNDERLINE_OFF ESC_SEQ "24m"

#define SCREEN_ALTERNATIVE_BUFFER_ENABLE ESC_SEQ "?1049h"
#define SCREEN_ALTERNATIVE_BUFFER_DISABLE ESC_SEQ "?1049l"

#define SCREEN_SCROLL_UP(x) ESC_SEQ "%dS", x
#define SCREEN_SCROLL_DOWN(x) ESC_SEQ "%dT", x

#define SCREEN_SCROLL_REGION(begin, end) ESC_SEQ "%d;%dr", begin, end
#define SCREEN_SCROLL_REGION_RESET ESC_SEQ ";r"

#define SET_RGB_FG_COLOR(r, g, b) ESC_SEQ "38;2;%d;%d;%dm", r, g, b
#define SET_RGB_BG_COLOR(r, g, b) ESC_SEQ "48;2;%d;%d;%dm", r, g, b

/// @brief Check if ANSI escape sequences are supported by the terminal
/// @return true if supported, false otherwise
bool CheckForAnsiSupport();

/// @brief Reset the cursor position to the top left corner of the terminal
void ResetCursor();

/// @brief Enable the alternative screen buffer
void EnableAlternativeBuffer();

/// @brief Disable the alternative screen buffer
void DisableAlternativeBuffer();

/// @brief Clear the screen line by line
void ClearScreenManual();

/// @brief Get the current cursor position
/// @param x Return value for the x coordinate
/// @param y Return value for the y coordinate
void GetCursorPosition(int* x, int* y);

/// @brief Set the cursor position
/// @param x The x coordinate
/// @param y The y coordinate
void SetCursorPosition(int x, int y);

// This is done to avoid conflicts with Windows API
#define HideCursor HideConsoleCursor
#define ShowCursor ShowConsoleCursor

/// @brief Clear the screen
void ClearScreen();
/// @brief Clear the line
void ClearLine();
/// @brief Reset colors to program defaults
void ResetColor();
/// @brief Hides the console cursor
void HideConsoleCursor();
/// @brief Shows the console cursor
void ShowConsoleCursor();
/// @brief Save the current cursor position
void SaveCursorPosition();
/// @brief Restore the saved cursor position
void RestoreCursorPosition();

#endif // _INC_ANSIHELPER_H
