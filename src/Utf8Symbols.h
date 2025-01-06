#ifndef _INC_UTF8SYMBOLS_H
#define _INC_UTF8SYMBOLS_H

#include "stdbool.h"

#define VERTICAL_LINE "%c%c%c", 0xE2, 0x95, 0x91 /*'║'*/
#define HORIZONTAL_LINE "%c%c%c", 0xE2, 0x95, 0x90 /*'═'*/
#define TOP_LEFT_CORNER "%c%c%c", 0xE2, 0x95, 0x94 /*'╔'*/
#define TOP_RIGHT_CORNER "%c%c%c", 0xE2, 0x95, 0x97 /*'╗'*/
#define BOTTOM_LEFT_CORNER "%c%c%c", 0xE2, 0x95, 0x9A /*'╚'*/
#define BOTTOM_RIGHT_CORNER "%c%c%c", 0xE2, 0x95, 0x9D /*'╝'*/
#define CROSS "%c%c%c", 0xE2, 0x95, 0xAC /*'╬'*/
#define TJUNCTION_LEFT "%c%c%c", 0xE2, 0x95, 0xA0 /*'╠'*/
#define TJUNCTION_RIGHT "%c%c%c", 0xE2, 0x95, 0xA3 /*'╣'*/
#define TJUNCTION_UP "%c%c%c", 0xE2, 0x95, 0xA6 /*'╦'*/
#define TJUNCTION_DOWN "%c%c%c", 0xE2, 0x95, 0xA9 /*'╩'*/

#define TJUNCTION_UP_SINGLE "%c%c%c", 0xE2, 0x95, 0xA7 /*'╧'*/
#define TJUNCTION_DOWN_SINGLE "%c%c%c", 0xE2, 0x95, 0xA4 /*'╤'*/

#define SINGLE_VERTICAL_LINE "%c%c%c", 0xE2, 0x94, 0x83 /*'┃'*/
#define SINGLE_HORIZONTAL_LINE "%c%c%c", 0xE2, 0x94, 0x81 /*'━'*/
#define SINGLE_TOP_LEFT_CORNER "%c%c%c", 0xE2, 0x94, 0x8F /*'┏'*/
#define SINGLE_TOP_RIGHT_CORNER "%c%c%c", 0xE2, 0x94, 0x93 /*'┓'*/
#define SINGLE_BOTTOM_LEFT_CORNER "%c%c%c", 0xE2, 0x94, 0x97 /*'┗'*/
#define SINGLE_BOTTOM_RIGHT_CORNER "%c%c%c", 0xE2, 0x94, 0x9B /*'┛'*/

#define SINGLE_BREAK_LEFT "%c%c%c", 0xE2, 0x94, 0xAB /*'┫'*/
#define SINGLE_BREAK_RIGHT "%c%c%c", 0xE2, 0x94, 0xA3 /*'┣'*/

#define SINGLE_LIGHT_VERTICAL_LINE "%c%c%c", 0xE2, 0x94, 0x82 /*'┃'*/

#define BLOCK_x_8(x) "%c%c%c", 0xE2, 0x96, (0x80+x) /*'▁'*/
#define BLOCK_1_8 "%c%c%c", 0xE2, 0x96, 0x81 /*'▁'*/
#define BLOCK_2_8 "%c%c%c", 0xE2, 0x96, 0x82 /*'▂'*/
#define BLOCK_3_8 "%c%c%c", 0xE2, 0x96, 0x83 /*'▃'*/
#define BLOCK_4_8 "%c%c%c", 0xE2, 0x96, 0x84 /*'▄'*/
#define BLOCK_5_8 "%c%c%c", 0xE2, 0x96, 0x85 /*'▅'*/
#define BLOCK_6_8 "%c%c%c", 0xE2, 0x96, 0x86 /*'▆'*/
#define BLOCK_7_8 "%c%c%c", 0xE2, 0x96, 0x87 /*'▇'*/
#define BLOCK_8_8 "%c%c%c", 0xE2, 0x96, 0x88 /*'█'*/

#define TRIANGLE "%c%c%c", 0xE2, 0x96, 0xB6 /*'▶'*/

#define DASHED_HORIZONTAL_LINE "%c%c%c", 0xE2, 0x95, 0x8C /*'╌'*/

void PrintGenericBorder(
    int width, 
    const char* leftFormat, 
    const int leftArg1, 
    const int leftArg2, 
    const int leftArg3,
    const char* middleFormat, 
    const int middleArg1, 
    const int middleArg2, 
    const int middleArg3,
    const char* rightFormat, 
    const int rightArg1, 
    const int rightArg2, 
    const int rightArg3);

void PrintGenericBorderEdges(
    int startX, int width, int y,
    const char* format, 
    const int arg1, 
    const int arg2, 
    const int arg3,
    bool clearInside);

#define PRINT_SINGLE_TOP_BORDER(width) PrintGenericBorder(width, SINGLE_TOP_LEFT_CORNER, SINGLE_HORIZONTAL_LINE, SINGLE_TOP_RIGHT_CORNER)
#define PRINT_SINGLE_BOTTOM_BORDER(width) PrintGenericBorder(width, SINGLE_BOTTOM_LEFT_CORNER, SINGLE_HORIZONTAL_LINE, SINGLE_BOTTOM_RIGHT_CORNER)
#define PRINT_SINGLE_TJUNCTION_BORDER(width) PrintGenericBorder(width, SINGLE_BREAK_RIGHT, SINGLE_HORIZONTAL_LINE, SINGLE_BREAK_LEFT)


#define PRINT_TOP_BORDER(width) PrintGenericBorder(width, TOP_LEFT_CORNER, HORIZONTAL_LINE, TOP_RIGHT_CORNER)
#define PRINT_BOTTOM_BORDER(width) PrintGenericBorder(width, BOTTOM_LEFT_CORNER, HORIZONTAL_LINE, BOTTOM_RIGHT_CORNER)
#define PRINT_TJUNCTION_BORDER(width) PrintGenericBorder(width, TJUNCTION_LEFT, HORIZONTAL_LINE, TJUNCTION_RIGHT)

#endif // _INC_UTF8SYMBOLS_H