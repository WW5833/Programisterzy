#include "TextHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "AnsiHelper.h"
#include "IOHelper.h"

int GetUTF8CharSize(const char* start) {
    if(start[0] == ESC_SEQ_CHAR) { // Skip ANSI
        int i = 2;
        start++;
        while(start[0] != '\0' && !isalpha(start[0])) {
            start++;
            i++;
        }
        return i;
    }

    if((start[0] & 0x80) == 0) { // ASCII character
        return 1;
    }

    // Determine the number of bytes in the UTF-8 character
    for (int j = 6; j >= 0; j--)
    {
        if((start[0] & (1 << j)) == 0) {
            return 7 - j;
        }
    }

    ExitAppWithErrorFormat(EXIT_FAILURE, "Invalid UTF-8 character: %c (%d)", start, start);
}

const char* _internal_GetNextChar(const char* c) {
    if(*c == ESC_SEQ_CHAR) {
        return c + GetUTF8CharSize(c);
    }

    if((*c & 0x80) == 0x00) {
        return c + 1;
    }

    if((*c & 0xC0) == 0x80) {
        ExitAppWithErrorMessage(EXIT_FAILURE, "Invalid UTF-8 character (Continuation byte received first)");
    }

    return c + GetUTF8CharSize(c);
}

const char* GetNextChar(const char* c) {
    const char* value = _internal_GetNextChar(c);
    if(value == NULL) {
        ExitAppWithErrorMessage(EXIT_FAILURE, "Invalid UTF-8 character (NULL received)");
    }

    if(value[0] == ESC_SEQ_CHAR) {
        return GetNextChar(value);
    }

    return value;
}

int GetCurrentCharSize(const char* stringStart, const char* c) {
    const char* start = GetCurrentChar(stringStart, c);

    return GetUTF8CharSize(start);
}

const char* GetCurrentChar(const char* stringStart, const char* c) {
    while(stringStart <= c) {
        if((*c & 0x80) == 0) { // ASCII character
            return c;
        }

        if((*c & 0xC0) != 0x80) { // Start of a new character
            return c;
        }

        c--;
    }

    return stringStart;
}

int GetStringCharCount(const char* start) {
    int count = 0;
    while (*start != '\0')
    {
        start = GetNextChar(start);
        count++;
    }

    return count;
}

int GetCharCount(const char* start, const char* end) {
    int count = 0;

    while (start < end)
    {
        start = GetNextChar(start);
        count++;
    }

    return count;
}

int _internal_PrintWrappedLine(const char* line, int width, int secondaryOffset, bool centerText, bool print)
{
    int byteLineLength;
    int lineLength = GetStringCharCount(line);

    int lineCount = 1;
    int currentWidth = 0;
    const char* wordStart = line;
    const char* current = line;
    const char* lineStart = line;
    int leftSpaces;

    while (*current != '\0') {
        const char* next = GetNextChar(current);

        if (*current == ' ' || *next == '\0' || *current == '\n') {
            int wordLength = GetCharCount(wordStart, current) + (*next == '\0' ? 1 : 0);
            if (currentWidth + wordLength > width || *current == '\n') {
                if(currentWidth + wordLength > width && *current == '\n') {
                    // Run new line again as this current new line will be because of text wrapping, not new line character
                    next = current;
                }
                else if(*current == '\n') {
                    wordStart = next;
                    currentWidth += wordLength + 1;
                    wordLength = 0;
                }

                lineLength = GetCharCount(lineStart, wordStart) - 1;

                if(print) {
                    if(centerText) {
                        leftSpaces = (width - lineLength) / 2;
                        if(leftSpaces > 0) printf(CSR_MOVE_RIGHT(leftSpaces));
                    }

                    byteLineLength = (int)(wordStart - lineStart) - 1;
                    printf("%.*s", byteLineLength, lineStart);

                    printf(CSR_MOVE_LEFT_0_DOWN1);
                    if(secondaryOffset > 0) printf(CSR_MOVE_RIGHT(secondaryOffset));
                }

                currentWidth = 0;
                lineStart = wordStart;
                lineCount++;
            }

            currentWidth += wordLength + 1;
            wordStart = next;
        }
        current = next;
    }

    if(print) {
        lineLength = GetCharCount(lineStart, wordStart);
        if(centerText) {
            leftSpaces = (width - lineLength) / 2;
            if(leftSpaces > 0) printf(CSR_MOVE_RIGHT(leftSpaces));
        }

        byteLineLength = (int)(current - lineStart);
        printf("%.*s", byteLineLength, lineStart);
    }

    return lineCount;
}

int GetWrappedLineCount(const char* line, int width)
{
    return _internal_PrintWrappedLine(line, width, 0, false, false);
}

int PrintWrappedLine(const char* line, int width, int secondaryOffset, bool centerText)
{
    return _internal_PrintWrappedLine(line, width, secondaryOffset, centerText, true);
}
