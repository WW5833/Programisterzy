#include "TextHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include "AnsiHelper.h"
#include "IOHelper.h"

int GetUTF8CharSize(char start) {
    if((start & 0x80) == 0) { // ASCII character
        return 1;
    }

    // Determine the number of bytes in the UTF-8 character
    for (int j = 6; j >= 0; j--)
    {
        if((start & (1 << j)) == 0) {
            return 7 - j;
        }
    }

    ExitAppWithErrorMessage(EXIT_FAILURE, "Invalid UTF-8 character 0xFF");
}

const char* GetNextChar(const char* c) {
    if((*c & 0x80) == 0x00) {
        return c + 1;
    }

    if((*c & 0xC0) == 0x80) {
        ExitAppWithErrorMessage(EXIT_FAILURE, "Invalid UTF-8 character (Continuation byte received first)");
    }

    return c + GetUTF8CharSize(c[0]);
}

int GetCurrentCharSize(const char* stringStart, const char* c) {
    const char* start = GetCurrentChar(stringStart, c);

    return GetUTF8CharSize(start[0]);
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
    if(lineLength <= width) {
        if(print) {
            if(centerText) {
                int leftSpaces = (width - lineLength) / 2;
                if(leftSpaces > 0) printf(CSR_MOVE_RIGHT(leftSpaces));
            }

            printf("%s", line);
        }
        return 1;
    }

    int lineCount = 1;
    int currentWidth = 0;
    const char* wordStart = line;
    const char* current = line;
    const char* lineStart = line;
    int leftSpaces;

    while (*current != '\0') {
        const char* next = GetNextChar(current);

        if (*current == ' ' || *next == '\0') {
            int wordLength = GetCharCount(wordStart, current) + (*next == '\0' ? 1 : 0);
            if (currentWidth + wordLength > width) {
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
