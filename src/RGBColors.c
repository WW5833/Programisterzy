#include "RGBColors.h"
#include "AnsiHelper.h"
#include "Settings.h"
#include <stdio.h>

#define RGB_GREEN          19, 161,  14
#define RGB_RED           197,  15,  31
#define RGB_BLUE            0,  55, 218
#define RGB_YELLOW        193, 156,   0
#define RGB_MAGENTA       136,  23, 152
#define RGB_CYAN           58, 150, 221
#define RGB_WHITE         255, 255, 255
#define RGB_GREY          204, 204, 204
#define RGB_BLACK           0,   0,   0

#define RGB_DARK_GREY      51,  51,  51
#define RGB_DARK_RED      136,   0,  21
#define RGB_DARK_GREEN     78, 154,   6
#define RGB_FULL_YELLOW   255, 255,   0
#define RGB_DARK_BLUE       6,  53, 204
#define RGB_LIGHT_MAGENTA 192,  96, 192
#define RGB_DARK_CYAN       6, 152, 204

#define COLOR_FG 38
#define COLOR_BG 48

#define SET_RGB_COLOR(mode, color) ESC_SEQ "%d;2;%d;%d;%dm", mode, color

extern Settings* LoadedSettings;

void SetTextColorToComplementBackground(int color) {
    bool darkText = false;
    bool noSwap = false;
    switch (color)
    {
        case RGB_ID_GREEN:
        case RGB_ID_RED:
        case RGB_ID_MAGENTA:
        case RGB_ID_CYAN:
        case RGB_ID_WHITE:
        case RGB_ID_GREY:
            darkText = true;
            break;

        case RGB_ID_BLUE:
            darkText = false;
            break;

        case RGB_ID_YELLOW:
            darkText = true;
            noSwap = true;
            break;

        default:
            darkText = true;
            break;
    }

    if(!noSwap && !LoadedSettings->DarkMode) {
        darkText = !darkText;
    }

    if(darkText) {
        printf(SET_RGB_COLOR(COLOR_FG, RGB_BLACK));
    }
    else {
        printf(SET_RGB_COLOR(COLOR_FG, RGB_WHITE));
    }
}

void internalSetColorRGBPreset(int color, int mode)
{
    switch (color)
    {
        case RGB_ID_GREEN:
            printf(SET_RGB_COLOR(mode, RGB_GREEN));
            break;
        case RGB_ID_RED:
            printf(SET_RGB_COLOR(mode, RGB_RED));
            break;
        case RGB_ID_BLUE:
            printf(SET_RGB_COLOR(mode, RGB_BLUE));
            break;
        case RGB_ID_YELLOW:
            printf(SET_RGB_COLOR(mode, RGB_YELLOW));
            break;
        case RGB_ID_MAGENTA:
            printf(SET_RGB_COLOR(mode, RGB_MAGENTA));
            break;
        case RGB_ID_CYAN:
            printf(SET_RGB_COLOR(mode, RGB_CYAN));
            break;
        case RGB_ID_WHITE:
            printf(SET_RGB_COLOR(mode, RGB_WHITE));
            break;
        case RGB_ID_GREY:
            printf(SET_RGB_COLOR(mode, RGB_GREY));
            break;
        default:
            printf(SET_RGB_COLOR(mode, RGB_WHITE));
            break;
    }
}

void internalSetColorRGBPresetWhiteMode(int color, int mode)
{
    switch (color)
    {
        case RGB_ID_GREEN:
            printf(SET_RGB_COLOR(mode, RGB_DARK_GREEN));
            break;
        case RGB_ID_RED:
            printf(SET_RGB_COLOR(mode, RGB_DARK_RED));
            break;
        case RGB_ID_BLUE:
            printf(SET_RGB_COLOR(mode, RGB_DARK_BLUE));
            break;
        case RGB_ID_YELLOW:
            printf(SET_RGB_COLOR(mode, RGB_FULL_YELLOW));
            break;
        case RGB_ID_MAGENTA:
            printf(SET_RGB_COLOR(mode, RGB_LIGHT_MAGENTA));
            break;
        case RGB_ID_CYAN:
            printf(SET_RGB_COLOR(mode, RGB_DARK_CYAN));
            break;
        case RGB_ID_WHITE:
            printf(SET_RGB_COLOR(mode, RGB_BLACK));
            break;
        case RGB_ID_GREY:
            printf(SET_RGB_COLOR(mode, RGB_DARK_GREY));
            break;
        default:
            printf(SET_RGB_COLOR(mode, RGB_WHITE));
            break;
    }
}

void SetColorRGBPreset(int color, bool background)
{
    if(background) {
        SetTextColorToComplementBackground(color);
    }

    int mode = background ? COLOR_BG : COLOR_FG;

    if(LoadedSettings->DarkMode) {
        internalSetColorRGBPreset(color, mode);
    }
    else {
        internalSetColorRGBPresetWhiteMode(color, mode);
    }
}

void SetResetColor() {
    if(LoadedSettings->DarkMode) {
        printf(SET_RGB_COLOR(COLOR_FG, RGB_GREY));
        printf(SET_RGB_COLOR(COLOR_BG, RGB_BLACK));
    }
    else {
        printf(SET_RGB_COLOR(COLOR_FG, RGB_BLACK));
        printf(SET_RGB_COLOR(COLOR_BG, RGB_GREY));
    }
}
