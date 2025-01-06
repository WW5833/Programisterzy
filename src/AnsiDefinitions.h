#ifndef _INC_ANSIDEFINITIONS_H
#define _INC_ANSIDEFINITIONS_H

#include <stdbool.h>

#define ESC_SEQ "\x1B["
#define CLR_LINE_END ESC_SEQ "0K" // Clear from cursor to end of line
#define CLR_LINE_START ESC_SEQ "1K" // Clear from cursor to start of line

#define CLR_SCRN ESC_SEQ "2J" // Clear screen

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

#define BOLD_ON ESC_SEQ "1m"
#define BOLD_OFF ESC_SEQ "22m"

#define DIM_ON ESC_SEQ "2m"
#define DIM_OFF ESC_SEQ "22m"

#define SCREEN_ALTERNATIVE_BUFFER_ENABLE ESC_SEQ "?1049h"
#define SCREEN_ALTERNATIVE_BUFFER_DISABLE ESC_SEQ "?1049l"

#define SCREEN_SCROLL_UP(x) ESC_SEQ "%dS", x
#define SCREEN_SCROLL_DOWN(x) ESC_SEQ "%dT", x

#define SCREEN_SCROLL_REGION(begin, end) ESC_SEQ "%d;%dr", begin, end
#define SCREEN_SCROLL_REGION_RESET ESC_SEQ ";r"

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

#define COLOR_BRIGHT_MOD 60

bool CheckForAnsiSupport();

void EnableAlternativeBuffer();
void DisableAlternativeBuffer();
void GetTerminalSize(int* x, int* y);
// This is here so that we can use it in the IOHelper, otherwise windows.h conflicts with ShowCursor/HideCursor

#endif // _INC_ANSIDEFINITIONS_H
