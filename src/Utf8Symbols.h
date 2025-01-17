#ifndef _INC_UTF8SYMBOLS_H
#define _INC_UTF8SYMBOLS_H

#include "stdbool.h"

#define VERTICAL_LINE "║"
#define HORIZONTAL_LINE "═"
#define TOP_LEFT_CORNER "╔"
#define TOP_RIGHT_CORNER "╗"
#define BOTTOM_LEFT_CORNER "╚"
#define BOTTOM_RIGHT_CORNER "╝"
#define CROSS "╬"
#define TJUNCTION_LEFT "╠"
#define TJUNCTION_RIGHT "╣"
#define TJUNCTION_UP "╦"
#define TJUNCTION_DOWN "╩"

#define TJUNCTION_UP_SINGLE "╧"
#define TJUNCTION_DOWN_SINGLE "╤"

#define SINGLE_VERTICAL_LINE "│"
#define SINGLE_HORIZONTAL_LINE "─"
#define SINGLE_TOP_LEFT_CORNER "┌"
#define SINGLE_TOP_RIGHT_CORNER "┐"
#define SINGLE_BOTTOM_LEFT_CORNER "└"
#define SINGLE_BOTTOM_RIGHT_CORNER "┘"

#define SINGLE_BREAK_LEFT "┤"
#define SINGLE_BREAK_RIGHT "├"
#define SINGLE_BREAK_TOP "┴"
#define SINGLE_BREAK_BOTTOM "┬"

#define SINGLE_CROSS "┼"

#define BLOCK_x_8(x) "%c%c%c", 0xE2, 0x96, (0x80+x)
#define BLOCK_1_8 "▁" /* Does not work on all terminals */
#define BLOCK_2_8 "▂" /* Does not work on all terminals */
#define BLOCK_3_8 "▃" /* Does not work on all terminals */
#define BLOCK_4_8 "▄"
#define BLOCK_5_8 "▅" /* Does not work on all terminals */
#define BLOCK_6_8 "▆" /* Does not work on all terminals */
#define BLOCK_7_8 "▇" /* Does not work on all terminals */
#define BLOCK_8_8 "█"

#define TRIANGLE "▶" /* Does not work on all terminals */

/// @brief Print generic border line
/// @param width Width of the border
/// @param leftCharacter Character on the left
/// @param middleCharacter Character in the middle
/// @param rightCharacter Character on the right
void PrintGenericBorder(
    int width,
    const char* leftCharacter,
    const char* middleCharacter,
    const char* rightCharacter);

/// @brief Print generic border edges
/// @param startX X to start from
/// @param width Width of the border
/// @param y Y of the border
/// @param character Character to use to print border
/// @param clearInside If true, in between the characters will be cleared
void PrintGenericBorderEdges(
    int startX, int width, int y,
    const char* character,
    bool clearInside);

/// @brief Print generic border edge on current line
/// @param startX X to start from
/// @param width Width of the border
/// @param character Character to use to print border
void PrintGenericBorderEdgesHere(
    int startX, int width,
    const char* character);

#define PRINT_SINGLE_TOP_BORDER(width) PrintGenericBorder(width, SINGLE_TOP_LEFT_CORNER, SINGLE_HORIZONTAL_LINE, SINGLE_TOP_RIGHT_CORNER)
#define PRINT_SINGLE_BOTTOM_BORDER(width) PrintGenericBorder(width, SINGLE_BOTTOM_LEFT_CORNER, SINGLE_HORIZONTAL_LINE, SINGLE_BOTTOM_RIGHT_CORNER)
#define PRINT_SINGLE_TJUNCTION_BORDER(width) PrintGenericBorder(width, SINGLE_BREAK_RIGHT, SINGLE_HORIZONTAL_LINE, SINGLE_BREAK_LEFT)

#define PRINT_TOP_BORDER(width) PrintGenericBorder(width, TOP_LEFT_CORNER, HORIZONTAL_LINE, TOP_RIGHT_CORNER)
#define PRINT_BOTTOM_BORDER(width) PrintGenericBorder(width, BOTTOM_LEFT_CORNER, HORIZONTAL_LINE, BOTTOM_RIGHT_CORNER)
#define PRINT_TJUNCTION_BORDER(width) PrintGenericBorder(width, TJUNCTION_LEFT, HORIZONTAL_LINE, TJUNCTION_RIGHT)

#endif // _INC_UTF8SYMBOLS_H
