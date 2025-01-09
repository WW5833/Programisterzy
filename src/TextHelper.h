#ifndef _INC_TEXTHELPER_H
#define _INC_TEXTHELPER_H

#include <stdbool.h>

const char* GetNextChar(const char* c);
const char* GetCurrentChar(const char* stringStart, const char* c);
int GetCurrentCharSize(const char* stringStart, const char* c);

int GetStringCharCount(const char* start);
int GetCharCount(const char* start, const char* end);
int GetWrappedLineCount(const char* line, int width);
int PrintWrappedLine(const char* line, int width, int secondaryOffset, bool centerText);

#endif // _INC_TEXTHELPER_H
