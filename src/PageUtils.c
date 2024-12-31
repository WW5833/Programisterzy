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

KeyInputType HandleInteractions(bool blocking) {
    if(!blocking && !_kbhit()) return KEY_NONE;
    unsigned char answer = (unsigned char)_getch();

    switch (answer)
    {
        case CTRL_C:
            ExitOnCtrlC();
            break;

        case '\015':
            return KEY_ENTER;
        case 'X':
            return KEY_X;
        case 'Y':
            return KEY_Y;
        case 'Z':
            return KEY_Z;

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
    printf("Exiting... (CTRL+C)\n");
    exit(EXIT_SUCCESS);
}

void WaitForEnter() {
    int c;
    while((c = _getch()) != '\015' && c != CTRL_C);
    if(c == CTRL_C) {
        ExitOnCtrlC();
    }
}

