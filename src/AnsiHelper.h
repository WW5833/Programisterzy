#ifndef _INC_ANSIHELPER_H
#define _INC_ANSIHELPER_H

#define ESC_SEQ "\x1B["
#define CLR_LINE_END ESC_SEQ "0K" // Clear from cursor to end of line
#define CLR_LINE_START ESC_SEQ "1K" // Clear from cursor to start of line

#define CSR_MOVE_UP(x) ESC_SEQ "%dA", x
#define CSR_MOVE_DOWN(x) ESC_SEQ "%dB", x
#define CSR_MOVE_RIGHT(x) ESC_SEQ "%dC", x
#define CSR_MOVE_LEFT(x) ESC_SEQ "%dD", x

#define CSR_MOVE_UP_1 ESC_SEQ "1A"
#define CSR_MOVE_DOWN_1 ESC_SEQ "1B"
#define CSR_MOVE_RIGHT_1 ESC_SEQ "1C"
#define CSR_MOVE_LEFT_1 ESC_SEQ "1D"

#define UNDERLINE_ON ESC_SEQ "4m"
#define UNDERLINE_OFF ESC_SEQ "24m"

#define BOLD_ON ESC_SEQ "1m"
#define BOLD_OFF ESC_SEQ "22m"

#define DIM_ON ESC_SEQ "2m"
#define DIM_OFF ESC_SEQ "22m"

#define COLOR_FG_BLACK 30
#define COLOR_FG_RED 31
#define COLOR_FG_GREEN 32
#define COLOR_FG_YELLOW 33
#define COLOR_FG_BLUE 34
#define COLOR_FG_MAGENTA 35
#define COLOR_FG_CYAN 36
#define COLOR_FG_WHITE 37
#define COLOR_FG_DEFAULT 39
#define COLOR_RESET 0

#define COLOR_BG_BLACK 40
#define COLOR_BG_RED 41
#define COLOR_BG_GREEN 42
#define COLOR_BG_YELLOW 43
#define COLOR_BG_BLUE 44
#define COLOR_BG_MAGENTA 45
#define COLOR_BG_CYAN 46
#define COLOR_BG_WHITE 47
#define COLOR_BG_DEFAULT 49

void GetCursorPosition(int* x, int* y);
void SetCursorPosition(int x, int y);
void GetTerminalSize(int* x, int* y);
void ClearScreen();
void ClearLine();
void SetColors(int fgColor, int bgColor);
void SetColor(int color);
void ResetColor();
void ResetCursor();
void HideCursor();
void ShowCursor();

#endif // _INC_ANSIHELPER_H