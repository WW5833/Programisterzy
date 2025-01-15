#ifndef _INC_TEXTHELPER_H
#define _INC_TEXTHELPER_H

#include <stdbool.h>

/// @brief Gets next character in string
/// @param c Start character
/// @return Next character
const char* GetNextChar(const char* c);
/// @brief Gets current character head (UTF8 first byte)
/// @param stringStart Start of the string
/// @param c Character to get head of
/// @return Current character head
const char* GetCurrentChar(const char* stringStart, const char* c);
/// @brief Gets size of the current character
/// @param stringStart Start of the string
/// @param c Character to get size of
/// @return Size of the character (1 for ASCI, 2-4 for UTF8, x for ANSI sequenses)
int GetCurrentCharSize(const char* stringStart, const char* c);

/// @brief Gets number of characters in a string (Count UTF8 as 1 character)
/// @param start String to count
/// @return Number of characters in the string
int GetStringCharCount(const char* start);
/// @brief Gets number of characters in a string (Count UTF8 as 1 character)
/// @param start From where to start counting
/// @param end Where end counting
/// @return Number of characters
int GetCharCount(const char* start, const char* end);
/// @brief Gets number of lines the text will take
/// @param text Line to check
/// @param width Width of space available for text
/// @return Number of lines
int GetWrappedLineCount(const char* text, int width);
/// @brief Prints the text with max width with text wrapping
/// @param text Text to print
/// @param width Max width of one line
/// @param secondaryOffset Left offset from the start of the screen for the next lines
/// @param centerText If true, the text will be centered
/// @return Number of lines used to print text
int PrintWrappedLine(const char* text, int width, int secondaryOffset, bool centerText);
/// @brief Gets the number of characters in the longest word in text
/// @param text Text to check
/// @return Number of characters
int GetMaxWordLength(const char* text);

#endif // _INC_TEXTHELPER_H
