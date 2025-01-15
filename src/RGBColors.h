#ifndef _INT_RGBCOLORS_H
#define _INT_RGBCOLORS_H

#include <stdbool.h>

#define RGB_COLOR_COUNT 8

#define RGB_ID_RED      0
#define RGB_ID_GREEN    1
#define RGB_ID_BLUE     2
#define RGB_ID_YELLOW   3
#define RGB_ID_MAGENTA  4
#define RGB_ID_CYAN     5
#define RGB_ID_GREY     6
#define RGB_ID_WHITE    7

/// @brief Set console color
/// @param color Color Id
/// @param background If true the color will be set for background
void SetColorRGBPreset(int color, bool background);
/// @brief Resets console color to defaults
void SetResetColor();

#endif // _INT_RGBCOLORS_H
