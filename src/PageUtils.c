#include "PageUtils.h"
#include "AnsiHelper.h"
#include <string.h>
#include <conio.h>

int GetWrappedLineCount(const char* line, int width)
{
    int lineLength = (int)strlen(line);
    if(lineLength <= width) {
        return 1;
    }

    int lineCount = 1;
    int currentWidth = 0;
    const char* wordStart = line;
    const char* current = line;

    while (*current != '\0') {
        if (*current == ' ' || *(current + 1) == '\0') {
            int wordLength = (int)(current - wordStart) + (*(current + 1) == '\0' ? 1 : 0);
            if (currentWidth + wordLength > width) {
                lineCount++;
                currentWidth = 0;
            }
            currentWidth += wordLength + 1;
            wordStart = current + 1;
        }
        current++;
    }

    return lineCount;
}

void PrintWrappedLine(const char* line, int width, int secondaryOffset, bool centerText)
{
    int lineLength = (int)strlen(line);
    if(lineLength <= width) {
        if(centerText) {
            printf(CSR_MOVE_RIGHT((width - lineLength) / 2));
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
        if (*current == ' ' || *(current + 1) == '\0') {
            int wordLength = (int)(current - wordStart) + (*(current + 1) == '\0' ? 1 : 0);
            if (currentWidth + wordLength > width) {

                if(centerText) {
                    lineLength = (int)(wordStart - lineStart) - 1;
                    rightSpaces = (width - lineLength) / 2;
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

            currentWidth += wordLength + 1;
            wordStart = current + 1;
        }
        current++;
    }

    if(centerText) {
        lineLength = (int)(current - lineStart);
        rightSpaces = (width - lineLength) / 2;
        if(rightSpaces > 0) printf(CSR_MOVE_RIGHT(rightSpaces));
        printf("%.*s", lineLength, lineStart);
    }
}

void GetWrappedLineCursorPosition(const char* line, int width, int position, int* cursorX, int* cusrorY)
{
    *cursorX = 0;
    *cusrorY = 0;

    int lineLength = (int)strlen(line);
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
        case 'X':
            return KEY_X;
        case 'Y':
            return KEY_Y;
        case 'Z':
            return KEY_Z;
        
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
    int c;

    SetCursorPosition(0, 999);

    while((c = _getch()) != ENTER && c != CTRL_C); // Enter or CTRL+C
    if(c == CTRL_C) {
        ExitOnCtrlC();
    }
}