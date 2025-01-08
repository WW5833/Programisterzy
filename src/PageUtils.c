#include "PageUtils.h"
#include "AnsiHelper.h"
#include "IOHelper.h"

int GetUTF8CharSize(char start) {
    // Determine the number of bytes in the UTF-8 character
    for (int j = 6; j >= 0; j--)
    {
        if((start & (1 << j)) == 0) {
            return 7 - j;
        }
    }

    fprintf(stderr, "Invalid UTF-8 character 0xFF\n");
    ExitApp(EXIT_FAILURE);
}

const char* GetNextChar(const char* c) {
    if((*c & 0x80) == 0x00) {
        return c + 1;
    }

    if((*c & 0xC0) == 0x80) {
        fprintf(stderr, "Invalid UTF-8 character (Continuation byte received first)\n");
        ExitApp(EXIT_FAILURE);
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

int GetWrappedLineCount(const char* line, int width)
{
    int lineLength = GetStringCharCount(line);
    if(lineLength <= width) {
        return 1;
    }

    int lineCount = 1;
    int currentWidth = 0;
    const char* wordStart = line;
    const char* current = line;

    while (*current != '\0') {
        const char* next = GetNextChar(current);

        if (*current == ' ' || *next == '\0') {
            int wordLength = GetCharCount(wordStart, current) + (*next == '\0' ? 1 : 0);
            if (currentWidth + wordLength > width) {
                lineCount++;
                currentWidth = 0;
            }
            currentWidth += wordLength + 1;
            wordStart = next;
        }

        current = next;
    }

    return lineCount;
}

int PrintWrappedLine(const char* line, int width, int secondaryOffset, bool centerText)
{
    int byteLineLength;
    int lineLength = GetStringCharCount(line);
    if(lineLength <= width) {
        if(centerText) {
            printf(CSR_MOVE_RIGHT((width - lineLength) / 2));
        }

        printf("%s", line);
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

                if(centerText) {
                    lineLength = GetCharCount(lineStart, wordStart) - 1;

                    leftSpaces = (width - lineLength) / 2;
                    if(leftSpaces > 0) printf(CSR_MOVE_RIGHT(leftSpaces));

                    byteLineLength = (int)(wordStart - lineStart) - 1;
                    printf("%.*s", byteLineLength, lineStart);
                }

                printf(CSR_MOVE_LEFT_0_DOWN1 CSR_MOVE_RIGHT(secondaryOffset));
                currentWidth = 0;
                lineStart = wordStart;
                lineCount++;
            }

            if(!centerText) {
                int byteWordLength = (int)(current - wordStart) + (*next == '\0' ? 1 : 0);
                printf("%.*s ", byteWordLength, wordStart);
            }

            currentWidth += wordLength + 1;
            wordStart = next;
        }
        current = next;
    }

    if(centerText) {
        lineLength = GetCharCount(lineStart, wordStart);

        leftSpaces = (width - lineLength) / 2;
        if(leftSpaces > 0) printf(CSR_MOVE_RIGHT(leftSpaces));

        byteLineLength = (int)(current - lineStart);
        printf("%.*s", byteLineLength, lineStart);
    }

    return lineCount;
}

KeyInputType HandleInteractions(bool blocking) {
    if(!blocking && !kbhit()) return KEY_NONE;

    char c = (char)getch();

    switch (c)
    {
        case CTRL_C:
            ExitOnCtrlC();
            break;

        case ENTER:
            return KEY_ENTER;
        case '1':
            return KEY_1;
        case '2':
            return KEY_2;
        case '3':
            return KEY_3;
        
        case 'R':
            return KEY_R;

        case ESC:
            return KEY_ESCAPE;

        case ESCAPE_CHAR:
            switch (getch())
            {
                case 72: // Arrow Up
                    return KEY_ARROW_UP;
                case 80: // Arrow Down
                    return KEY_ARROW_DOWN;
                case 77: // Arrow Right
                    return KEY_ARROW_RIGHT;
                case 75: // Arrow Left
                    return KEY_ARROW_LEFT;

                default:
                    break;
            }
            break;

        default:
            break;
    }

    return KEY_NONE;
}

void ExitOnCtrlC() {
    // printf("Exiting... (CTRL+C)\t");
    ExitApp(EXIT_SUCCESS);
}

void WaitForEnter() {
    WaitForKeys(ENTER);
}

char _internal_WaitForKeys(int count, char* keys)
{
    int c;
    while(true) {
        c = WaitForAnyInput();

        if(c == CTRL_C) {
            ExitOnCtrlC();
        }

        for (int i = 0; i < count; i++)
        {
            if(keys[i] == ANY_MOUSE_BUTTON && c == INT_MAX) {
                return ANY_MOUSE_BUTTON;
            }
            else if(c == keys[i]) {
                return (char)c;
            }
        }
    }
}
