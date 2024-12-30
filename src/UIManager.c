#include "UIManager.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "QuizManager.h"
#include "AnsiHelper.h"

#define VERTICAL_LINE "%c%c%c", 0xE2, 0x95, 0x91 /*'║'*/
#define HORIZONTAL_LINE "%c%c%c", 0xE2, 0x95, 0x90 /*'═'*/
#define TOP_LEFT_CORNER "%c%c%c", 0xE2, 0x95, 0x94 /*'╔'*/
#define TOP_RIGHT_CORNER "%c%c%c", 0xE2, 0x95, 0x97 /*'╗'*/
#define BOTTOM_LEFT_CORNER "%c%c%c", 0xE2, 0x95, 0x9A /*'╚'*/
#define BOTTOM_RIGHT_CORNER "%c%c%c", 0xE2, 0x95, 0x9D /*'╝'*/
#define CROSS "%c%c%c", 0xE2, 0x95, 0xAC /*'╬'*/
#define TJUNCTION_LEFT "%c%c%c", 0xE2, 0x95, 0xA0 /*'╠'*/
#define TJUNCTION_RIGHT "%c%c%c", 0xE2, 0x95, 0xA3 /*'╣'*/
#define TJUNCTION_UP "%c%c%c", 0xE2, 0x95, 0xA6 /*'╦'*/
#define TJUNCTION_DOWN "%c%c%c", 0xE2, 0x95, 0xA9 /*'╩'*/

#define SINGLE_VERTICAL_LINE "%c%c%c", 0xE2, 0x94, 0x83 /*'┃'*/
#define SINGLE_HORIZONTAL_LINE "%c%c%c", 0xE2, 0x94, 0x81 /*'━'*/
#define SINGLE_TOP_LEFT_CORNER "%c%c%c", 0xE2, 0x94, 0x8F /*'┏'*/
#define SINGLE_TOP_RIGHT_CORNER "%c%c%c", 0xE2, 0x94, 0x93 /*'┓'*/
#define SINGLE_BOTTOM_LEFT_CORNER "%c%c%c", 0xE2, 0x94, 0x97 /*'┗'*/
#define SINGLE_BOTTOM_RIGHT_CORNER "%c%c%c", 0xE2, 0x94, 0x9B /*'┛'*/

#define SINGLE_BREAK_LEFT "%c%c%c", 0xE2, 0x94, 0xAB /*'┫'*/
#define SINGLE_BREAK_RIGHT "%c%c%c", 0xE2, 0x94, 0xA3 /*'┣'*/

#define PRINT_TOP_BORDER(width) {\
    printf(TOP_LEFT_CORNER); \
    for (int _i = 0; _i < width - 2; _i++) \
        printf(HORIZONTAL_LINE); \
    printf(TOP_RIGHT_CORNER); printf("\n"); }

#define PRINT_BOTTOM_BORDER(width) {\
    printf(BOTTOM_LEFT_CORNER); \
    for (int _i = 0; _i < width - 2; _i++) \
        printf(HORIZONTAL_LINE); \
    printf(BOTTOM_RIGHT_CORNER); printf("\n"); }

#define PRINT_MIDDLE_BORDER(width) {\
    printf(TJUNCTION_LEFT); \
    for (int _i = 0; _i < width - 2; _i++) \
        printf(HORIZONTAL_LINE); \
    printf(TJUNCTION_RIGHT); printf("\n"); }

#define PRINT_VERTICAL_LINES(width, i) {\
    SetCursorPosition(0, i); \
    printf(VERTICAL_LINE); \
    SetCursorPosition(width, i); \
    printf(VERTICAL_LINE); }


#define PRINT_SINGLE_TOP_BORDER(width) {\
    printf(SINGLE_TOP_LEFT_CORNER); \
    for (int _i = 0; _i < width - 1; _i++) \
        printf(SINGLE_HORIZONTAL_LINE); \
    printf(SINGLE_TOP_RIGHT_CORNER); printf("\n"); }

#define PRINT_SINGLE_BOTTOM_BORDER(width) {\
    printf(SINGLE_BOTTOM_LEFT_CORNER); \
    for (int _i = 0; _i < width - 1; _i++) \
        printf(SINGLE_HORIZONTAL_LINE); \
    printf(SINGLE_BOTTOM_RIGHT_CORNER); printf("\n"); }

#define PRINT_SINGLE_VERTICAL_LINES(width, startX, targetY) {\
    SetCursorPosition(startX, targetY); \
    printf(SINGLE_VERTICAL_LINE); \
    SetCursorPosition(startX + width, targetY); \
    printf(SINGLE_VERTICAL_LINE); }


#define MAX(x, y) ((x) > (y) ? (x) : (y))

void PrintMainMenu()
{
    ResetColor();
    ClearScreen();
    // int x, y;
    // GetTerminalSize(&x, &y);
    // printf("[DEBUG] TerminalSize (%d, %d)\n", x, y);
    printf("Witaj w Programisterach!\n");
    printf("1. Rozpocznij quiz\n");
    printf("2. Dodaj pytanie\n");
    printf("3. Tablica wyników\n");
    printf("Q. Wyjdź\n");
}

int GetWrappedLineCount(const char* line, int width, int offset, int secondaryOffset)
{
    int lineCount = 1;
    int lineLength = (int)strlen(line) + offset;
    if(lineLength <= width) {
        return lineCount;
    }

    int currentWidth = offset;
    const char* wordStart = line;
    const char* current = line;

    while (*current != '\0') {
        if (*current == ' ' || *(current + 1) == '\0') {
            int wordLength = (int)(current - wordStart) + (*(current + 1) == '\0' ? 1 : 0);
            if (currentWidth + wordLength > width) {
                lineCount++;
                currentWidth = secondaryOffset;
            }
            currentWidth += wordLength + 1;
            wordStart = current + 1;
        }
        current++;
    }

    return lineCount;
}


void PrintWrappedLine(const char* line, int width, int offset, int secondaryOffset, int realSecondaryOffset)
{
    int lineLength = (int)strlen(line) + offset;
    if(lineLength <= width) {
        printf("%s", line);
        return;
    }

    int currentWidth = offset;
    const char* wordStart = line;
    const char* current = line;

    while (*current != '\0') {
        if (*current == ' ' || *(current + 1) == '\0') {
            int wordLength = (int)(current - wordStart) + (*(current + 1) == '\0' ? 1 : 0);
            if (currentWidth + wordLength > width) {
                printf(CSR_MOVE_LEFT_0_DOWN1);
                printf(CSR_MOVE_RIGHT(realSecondaryOffset));
                currentWidth = secondaryOffset;
            }
            printf("%.*s ", wordLength, wordStart);
            currentWidth += wordLength + 1;
            wordStart = current + 1;
        }
        current++;
    }
}

void PrintWrappedLineJust(const char* line, int width, int offset, int secondaryOffset, int realSecondaryOffset)
{
    int lineLength = (int)strlen(line);
    if(lineLength <= width - offset) {
        printf(CSR_MOVE_RIGHT((width - lineLength) / 2));
        printf("%s", line);
        return;
    }

    int currentWidth = offset;
    const char* wordStart = line;
    const char* current = line;
    const char* lineStart = line;
    int rightSpaces;

    while (*current != '\0') {
        if (*current == ' ' || *(current + 1) == '\0') {
            int wordLength = (int)(current - wordStart) + (*(current + 1) == '\0' ? 1 : 0);
            if (currentWidth + wordLength > width) {
                lineLength = (int)(wordStart - lineStart);
                rightSpaces = (width - lineLength) / 2;
                printf(CSR_MOVE_RIGHT(rightSpaces));
                printf("%.*s", lineLength, lineStart);

                printf(CSR_MOVE_LEFT_0_DOWN1);
                printf(CSR_MOVE_RIGHT(realSecondaryOffset));
                currentWidth = secondaryOffset;
                lineStart = wordStart;
            }
            //printf("%.*s ", wordLength, wordStart);
            currentWidth += wordLength + 1;
            wordStart = current + 1;
        }
        current++;
    }

    lineLength = (int)(current - lineStart);
    rightSpaces = (width - lineLength) / 2;
    printf(CSR_MOVE_RIGHT(rightSpaces));
    printf("%.*s", lineLength, lineStart);
}

void UILoop_MainMenu();
void UILoop_Quiz();
bool UILoop_QuizQuestion(Question* question, int number, bool* abilities, bool* outCorrect, char* outAnswer);

void UILoop_MainMenu()
{
    char choice = '\0';
    while (choice != 'q')
    {
        PrintMainMenu();
        choice = (char)_getch();
        switch (choice)
        {
            case '1':
                UILoop_Quiz();
                break;
            case '2':
                break;
            case '3':
                break;
            case 'Q':
            case 'q':
            case '\03':
                exit(EXIT_SUCCESS);
            default:
                break;
        }
    }
}

void ReadUsername(char username[30]) {
    char c;
    for (int i = 0; i < 30; i++)
    {
        c = (char)_getch();
        if(c == '\0') break;
        if(c == '\03') {
            printf("Exiting... (CTRL+C)\n");
            exit(EXIT_SUCCESS);
        }
        if(c == 13) {
            username[i] = '\0';
            printf("\n");
            break;
        }

        if(c == ' ') {
            username[i] = '_';
            printf("_");
            continue;
        }

        if(c == '\b') {
            if(i == 0) continue;
            i -= 2;
            printf(CSR_MOVE_LEFT(1));
            printf(" " );
            printf(CSR_MOVE_LEFT(1));
            continue;
        }

        if(c >= 'a' && c <= 'z') c -= 32;
        username[i] = c;
        printf("%c", c);
    }
}

void UILoop_Quiz()
{
    char username[30];

    printf("Podaj swoje imie: ");
    ReadUsername(username);

    QuizData* quiz = GenerateQuiz(username);
    if(quiz == NULL) return;

    bool correct = false;
    for(int i = 0; i < 10; i++) {
        Question* q = ListGetAt(quiz->questions, i);
        if(!UILoop_QuizQuestion(q, i + 1, &quiz->abulitiesUsed[0], &correct, &quiz->answers[i])) {
            printf("Przegrałeś :/\n");
            while(_getch() != '\015');
            break;
        }
    }

    DestroyQuiz(quiz);

    UILoop_MainMenu();
}

void PrintSingleAnsBlock(int beginY, int ansWidth, int lineCount, char letter, bool color, int colorFg, int colorBg, int beginX) {
    if(color) SetColors(colorFg, colorBg);
    SetCursorPosition(beginX, beginY);

    int startText = ansWidth/2 - 7 - 1;
    int endText = ansWidth/2 + 6 - 1;
    printf(SINGLE_TOP_LEFT_CORNER);
    for (int i = 0; i < ansWidth - 1; i++) {
        if(i == startText) printf(SINGLE_BREAK_LEFT);
        else if(i == endText) printf(SINGLE_BREAK_RIGHT);
        else if(i > startText && i < endText) printf(CSR_MOVE_RIGHT_1);
        else printf(SINGLE_HORIZONTAL_LINE);
    }
    printf(SINGLE_TOP_RIGHT_CORNER);

    for (int i = 0; i < lineCount + 2; i++)
        PRINT_SINGLE_VERTICAL_LINES(ansWidth, beginX, beginY + i + 1);

    SetCursorPosition(beginX, beginY + lineCount + 3);
    PRINT_SINGLE_BOTTOM_BORDER(ansWidth);
    if(color) ResetColor();

    // Print letter
    SetCursorPosition(beginX + ansWidth/2 - 7 + 1, beginY);

    SetColor(COLOR_FG_CYAN);
    printf("Odpowiedź: %c", letter);

    ResetColor();
}

void PrintDoubleAnsBlock(int beginY, int ansWidth, int lineCount, char letter, int color, int colorFg, int colorBg) {
    PrintSingleAnsBlock(beginY, ansWidth, lineCount, letter, color == 1, colorFg, colorBg, 3);
    PrintSingleAnsBlock(beginY, ansWidth, lineCount, letter + 1, color == 2, colorFg, colorBg, 3 + ansWidth + 3);
}

void PrintDoubleAnsBlockDelta(int beginY, int ansWidth, int lineCount, char letter, int color, int colorFg, int colorBg, int oldColor) {
    if(color == oldColor) return;
    if(color == 1 || oldColor == 1) PrintSingleAnsBlock(beginY, ansWidth, lineCount, letter, color == 1, colorFg, colorBg, 3);
    if(color == 2 || oldColor == 2) PrintSingleAnsBlock(beginY, ansWidth, lineCount, letter + 1, color == 2, colorFg, colorBg, 3 + ansWidth + 3);
}

bool UILoop_QuizQuestion(Question* question, int number, bool* abilities, bool* outCorrect, char* outAnswer) { 
    int offset = rand() % 4;

    int questionEndLine;
    int endAnsLine;
    int tmp;
    int terminalX, terminalY;
    GetTerminalSize(&terminalX, &terminalY);

    ClearScreen();

    PRINT_TOP_BORDER(terminalX);
    printf("  Pytanie %2d: ", number);
    PrintWrappedLine(question->Content, terminalX - 2, 15, 14, 14);
    printf("\n");
    PRINT_MIDDLE_BORDER(terminalX);
    GetCursorPosition(&tmp, &questionEndLine);

    for (int i = 2; i < questionEndLine - 1; i++)
        PRINT_VERTICAL_LINES(terminalX, i);

    SetCursorPosition(0, questionEndLine);
    
    int ansWidthLimit = (terminalX - 19) / 2;
    int ansLineCountA = GetWrappedLineCount(question->Answer[(0 + offset) % 4], ansWidthLimit, 0, 0);
    int ansLineCountB = GetWrappedLineCount(question->Answer[(1 + offset) % 4], ansWidthLimit, 0, 0);
    int ansLineCountMaxAB = MAX(ansLineCountA, ansLineCountB);
    int ansLineCountC = GetWrappedLineCount(question->Answer[(2 + offset) % 4], ansWidthLimit, 0, 0);
    int ansLineCountD = GetWrappedLineCount(question->Answer[(3 + offset) % 4], ansWidthLimit, 0, 0);
    int ansLineCountMaxCD = MAX(ansLineCountC, ansLineCountD);

    int ansWidth = ansWidthLimit + 6;

    PrintDoubleAnsBlock(questionEndLine, ansWidth, ansLineCountMaxAB, 'A', 1, COLOR_FG_YELLOW, COLOR_BG_BLACK);
    PrintDoubleAnsBlock(questionEndLine + ansLineCountMaxAB + 4, ansWidth, ansLineCountMaxCD, 'C', 0, COLOR_FG_YELLOW, COLOR_BG_BLACK);

    // Print Answer 1
    SetCursorPosition(5, questionEndLine + 2);
    PrintWrappedLineJust(question->Answer[(0 + offset) % 4], ansWidthLimit + 2, 2, 2, 5);
    // Print Answer 2
    SetCursorPosition(5 + ansWidthLimit + 9, questionEndLine + 2);
    PrintWrappedLineJust(question->Answer[(1 + offset) % 4], ansWidthLimit + 2, 2, 2, 5 + ansWidthLimit + 9);

    // Print Answer 3
    SetCursorPosition(5, questionEndLine + ansLineCountMaxAB + 6);
    PrintWrappedLineJust(question->Answer[(2 + offset) % 4], ansWidthLimit + 2, 2, 2, 5);
    // Print Answer 4
    SetCursorPosition(5 + ansWidthLimit + 9, questionEndLine + ansLineCountMaxAB + 6);
    PrintWrappedLineJust(question->Answer[(3 + offset) % 4], ansWidthLimit + 2, 2, 2, 5 + ansWidthLimit + 9);

    endAnsLine = questionEndLine + ansLineCountMaxAB + ansLineCountMaxCD + 8;

    for (int i = questionEndLine; i < endAnsLine; i++)
        PRINT_VERTICAL_LINES(terminalX, i);
    
    SetCursorPosition(0, endAnsLine);
    PRINT_MIDDLE_BORDER(terminalX);
    
    if(!abilities[0])
        printf("  X: Help1\n");
    else
        printf("  X: Already used\n");

    PRINT_VERTICAL_LINES(terminalX, endAnsLine + 1);

    if(!abilities[1])
        printf("  Y: Help2\n");
    else
        printf("  Y: Already used\n");

    PRINT_VERTICAL_LINES(terminalX, endAnsLine + 2);

    if(!abilities[2])
        printf("  Z: Help3\n");
    else
        printf("  Z: Already used\n");

    PRINT_VERTICAL_LINES(terminalX, endAnsLine + 3);

    PRINT_BOTTOM_BORDER(terminalX);
    printf("\n");

    int selected = 0;
    bool confirmed = false;
    
    while (true)
    {
        HideCursor();
        SetCursorPosition(2, questionEndLine + 7);

        unsigned char answer = (unsigned char)_getch();

        if(answer == '\03') { // CTRL+C
            printf("Exiting... (CTRL+C)\n");
            exit(EXIT_SUCCESS);
        }

        if(answer == 224) {
            int oldSel = selected;
            switch (_getch())
            {
                case 72: // Arrow Up
                    if(selected == 2 || selected == 3) selected -= 2;
                    else continue;
                    confirmed = false;
                    break;
                case 80: // Arrow Down
                    if(selected == 0 || selected == 1) selected += 2;
                    else continue;
                    confirmed = false;
                    break;
                case 77: // Arrow Right
                    if(selected == 0 || selected == 2) selected += 1;
                    else continue;
                    confirmed = false;
                    break;
                case 75: // Arrow Left
                    if(selected == 1 || selected == 3) selected -= 1;
                    else continue;
                    confirmed = false;
                    break;
                default:
                    break;
            }
            PrintDoubleAnsBlockDelta(questionEndLine, ansWidth, ansLineCountMaxAB, 'A', selected + 1, COLOR_FG_YELLOW, COLOR_BG_BLACK, oldSel + 1);
            PrintDoubleAnsBlockDelta(questionEndLine + ansLineCountMaxAB + 4, ansWidth, ansLineCountMaxCD, 'C', selected - 1, COLOR_FG_YELLOW, COLOR_BG_BLACK, oldSel - 1);
            continue;
        }

        if(answer == '\015') {
            if(confirmed) {
                int answerIndex = selected;
                answerIndex += offset;
                answerIndex = (answerIndex + 4) % 4;

                *outAnswer = (char)answerIndex;

                *outCorrect = answerIndex == 0;

                int fgColor = *outCorrect ? COLOR_FG_GREEN : COLOR_FG_RED;

                PrintDoubleAnsBlock(questionEndLine, ansWidth, ansLineCountMaxAB, 'A', selected + 1, fgColor, COLOR_BG_DEFAULT);
                PrintDoubleAnsBlock(questionEndLine + ansLineCountMaxAB + 4, ansWidth, ansLineCountMaxCD, 'C', selected - 1, fgColor, COLOR_BG_DEFAULT);

                ResetColor();

                while(_getch() != '\015');

                SetCursorPosition(0, questionEndLine + 10);
                return *outCorrect;
            }

            PrintDoubleAnsBlock(questionEndLine, ansWidth, ansLineCountMaxAB, 'A', selected + 1, COLOR_FG_CYAN, COLOR_BG_DEFAULT);
            PrintDoubleAnsBlock(questionEndLine + ansLineCountMaxAB + 4, ansWidth, ansLineCountMaxCD, 'C', selected - 1, COLOR_FG_CYAN, COLOR_BG_DEFAULT);
            confirmed = true;
            continue;
        }

        if(answer == 'X' && !abilities[0]) {
            SetCursorPosition(3, questionEndLine + 5);
            printf("X: %s\n", question->Help);
            abilities[0] = true;
            continue;
        }

        if(answer == 'Y' && !abilities[1]) {
            SetCursorPosition(3, questionEndLine + 6);
            printf("Y: %s\n", question->Help);
            abilities[1] = true;
            continue;
        }

        if(answer == 'Z' && !abilities[2]) {
            SetCursorPosition(3, questionEndLine + 7);
            printf("Z: %s\n", question->Help);
            abilities[2] = true;
            continue;
        }
    }
}
