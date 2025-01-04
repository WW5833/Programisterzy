#include "PageUtils.h"
#include "AnsiHelper.h"
#include <conio.h>

const char* GetNextChar(const char* c) {
    if((*c & 0x80) == 0x00) {
        return c + 1;
    }

    if((*c & 0xC0) == 0x80) {
        fprintf(stderr, "Invalid UTF-8 character (Continuation byte received first)\n");
        exit(EXIT_FAILURE);
    }

    for (int j = 6; j >= 0; j--)
    {
        if((*c & (1 << j)) == 0) {
            return c + (7 - j);
        }
    }

    fprintf(stderr, "Invalid UTF-8 character 0xFF\n");
    exit(EXIT_FAILURE);
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

void PrintWrappedLine(const char* line, int width, int secondaryOffset, bool centerText)
{
    int lineLength;
    int realLineLength = GetStringCharCount(line);
    if(realLineLength <= width) {
        if(centerText) {
            printf(CSR_MOVE_RIGHT((width - realLineLength) / 2));
        }

        printf("%s", line);
        return;
    }

    int currentWidth = 0;
    const char* wordStart = line;
    const char* current = line;
    const char* lineStart = line;
    int rightSpaces;

    while (*current != '\0') {
        const char* next = GetNextChar(current);

        if (*current == ' ' || *next == '\0') {
            int realWordLength = GetCharCount(wordStart, current) + (*next == '\0' ? 1 : 0);
            int wordLength = (int)(current - wordStart) + (*next == '\0' ? 1 : 0);
            if (currentWidth + realWordLength > width) {

                if(centerText) {
                    realLineLength = GetCharCount(lineStart, wordStart) - 1;
                    lineLength = (int)(wordStart - lineStart) - 1;
                    rightSpaces = (width - realLineLength) / 2;
                    printf(CSR_MOVE_RIGHT(rightSpaces));
                    printf("%.*s", lineLength, lineStart);
                }

                printf(CSR_MOVE_LEFT_0_DOWN1 CSR_MOVE_RIGHT(secondaryOffset));
                currentWidth = 0;
                lineStart = wordStart;
            }

            if(!centerText) {
                printf("%.*s ", wordLength, wordStart);
            }

            currentWidth += realWordLength + 1;
            wordStart = next;
        }
        current = next;
    }

    if(centerText) {
        realLineLength = GetCharCount(lineStart, wordStart);
        lineLength = (int)(current - lineStart);
        rightSpaces = (width - realLineLength) / 2;
        if(rightSpaces > 0) printf(CSR_MOVE_RIGHT(rightSpaces));
        printf("%.*s", lineLength, lineStart);
    }
}

// Todo: Maybe add support for UTF-8 characters
void GetWrappedLineCursorPosition(const char* line, int width, int position, int* cursorX, int* cusrorY)
{
    *cursorX = 0;
    *cusrorY = 0;

    int lineLength = GetStringCharCount(line);
    if(lineLength <= width) {
        *cursorX = position;
        return;
    }

    int index = 0;
    int currentWidth = 0;
    const char* wordStart = line;
    const char* current = line;

    while (*current != '\0') {
        if (*current == ' ' || *(current + 1) == '\0') {
            int wordLength = (int)(current - wordStart) + (*(current + 1) == '\0' ? 1 : 0);
            if (currentWidth + wordLength > width) {
                *cusrorY = *cusrorY + 1;
                *cursorX = 0;

                currentWidth = 0;
            }

            *cursorX = *cursorX + wordLength + 1;

            currentWidth += wordLength + 1;
            wordStart = current + 1;

            if(index >= position) {
                if(*cursorX > (index - position)) {
                    *cursorX = *cursorX - (index - position);
                    return;
                }

                SetCursorPosition(0, 0);
                printf("The code has died :/, index: %d, position: %d, cursorX: %d\n", index, position, *cursorX);
                exit(EXIT_FAILURE);
            }
        }
        current++;
        index++;
    }
}

KeyInputType HandleInteractions(bool blocking) {
    if(!blocking && !_kbhit()) return KEY_NONE;

    SetCursorPosition(0, 999);

    int c = _getch();

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

        case 224:
            switch (_getch())
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
    exit(EXIT_SUCCESS);
}

void WaitForEnter() {
    WaitForKeys(ENTER);
}

char _internal_WaitForKeys(int count, char* keys)
{
    SetCursorPosition(0, 999);

    int c;
    while(true) {
        c = _getch();

        if(c == CTRL_C) {
            ExitOnCtrlC();
        }

        for (int i = 0; i < count; i++)
        {
            if(c == keys[i]) {
                return (char)c;
            }
        }
    }
}
