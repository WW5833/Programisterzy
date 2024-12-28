#include "UIManager.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include "QuizManager.h"
#include "AnsiHelper.h"

#define VERTICAL_LINE 186
#define HORIZONTAL_LINE 205
#define TOP_LEFT_CORNER 201
#define TOP_RIGHT_CORNER 187
#define BOTTOM_LEFT_CORNER 200
#define BOTTOM_RIGHT_CORNER 188
#define CROSS 206
#define TJUNCTION_LEFT 204
#define TJUNCTION_RIGHT 185
#define TJUNCTION_UP 203
#define TJUNCTION_DOWN 202

#define PRINT_TOP_BORDER(width) \
    printf("%c", TOP_LEFT_CORNER); {\
    for (int _i = 0; _i < width - 2; _i++) \
        printf("%c", HORIZONTAL_LINE); \
    printf("%c\n", TOP_RIGHT_CORNER); }

#define PRINT_BOTTOM_BORDER(width) {\
    printf("%c", BOTTOM_LEFT_CORNER); \
    for (int _i = 0; _i < width - 2; _i++) \
        printf("%c", HORIZONTAL_LINE); \
    printf("%c\n", BOTTOM_RIGHT_CORNER); }

#define PRINT_MIDDLE_BORDER(width) {\
    printf("%c", TJUNCTION_LEFT); \
    for (int _i = 0; _i < width - 2; _i++) \
        printf("%c", HORIZONTAL_LINE); \
    printf("%c\n", TJUNCTION_RIGHT); }

#define PRINT_VERTICAL_LINES(width, i) {\
    SetCursorPosition(0, i); \
    printf("%c", VERTICAL_LINE); \
    SetCursorPosition(width, i); \
    printf("%c", VERTICAL_LINE); }


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
    printf("3. Tablica wynikow\n");
    printf("Q. Wyjdz\n");
}

void PrintWrappedLine(const char* line, int width, int offset, int secondaryOffset)
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
                printf("\n%*s", secondaryOffset, "");
                currentWidth = secondaryOffset;
            }
            printf("%.*s ", wordLength, wordStart);
            currentWidth += wordLength + 1;
            wordStart = current + 1;
        }
        current++;
    }
    
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

    for(int i = 0; i < 10; i++) {
        Question* q = ListGetAt(quiz->questions, i);
        // PrintQuestion(q, i + 1, 0);
        // getchar();
        if(!UILoop_QuizQuestion(q, i + 1, &quiz->abulitiesUsed[0], &quiz->asnwersCorrect[i], &quiz->answers[i])) {
            printf("Przegrales :/\n");
            getchar();
            break;
        }
    }

    DestroyQuiz(quiz);

    UILoop_MainMenu();
}

bool UILoop_QuizQuestion(Question* question, int number, bool* abilities, bool* outCorrect, char* outAnswer) { 
    int offset = rand() % 4;

    int questionEndLine;
    int tmp;
    int terminalX, terminalY;
    GetTerminalSize(&terminalX, &terminalY);

    ClearScreen();

    PRINT_TOP_BORDER(terminalX);
    printf("  Pytanie %2d: ", number);
    PrintWrappedLine(question->Content, terminalX - 2, 15, 14);
    printf("\n");
    PRINT_MIDDLE_BORDER(terminalX);
    GetCursorPosition(&tmp, &questionEndLine);

    for (int i = 2; i < questionEndLine - 1; i++)
        PRINT_VERTICAL_LINES(terminalX, i);

    SetCursorPosition(0, questionEndLine);

    printf("  " UNDERLINE_ON "A: %s" UNDERLINE_OFF "\n", question->Answer[(0 + offset) % 4]);
    PRINT_VERTICAL_LINES(terminalX, questionEndLine + 0);

    printf("  B: %s\n", question->Answer[(1 + offset) % 4]);
    PRINT_VERTICAL_LINES(terminalX, questionEndLine + 1);

    printf("  C: %s\n", question->Answer[(2 + offset) % 4]);
    PRINT_VERTICAL_LINES(terminalX, questionEndLine + 2);

    printf("  D: %s\n", question->Answer[(3 + offset) % 4]);
    PRINT_VERTICAL_LINES(terminalX, questionEndLine + 3);
    
    PRINT_MIDDLE_BORDER(terminalX);

    if(!abilities[0])
        printf("  X: Help1\n");
    else
        printf("  X: Already used\n");

    PRINT_VERTICAL_LINES(terminalX, questionEndLine + 5);

    if(!abilities[1])
        printf("  Y: Help2\n");
    else
        printf("  Y: Already used\n");

    PRINT_VERTICAL_LINES(terminalX, questionEndLine + 6);

    if(!abilities[2])
        printf("  Z: Help3\n");
    else
        printf("  Z: Already used\n");

    PRINT_VERTICAL_LINES(terminalX, questionEndLine + 7);

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
            SetCursorPosition(3, questionEndLine + selected);
            printf("%c: %s", 'A' + selected, question->Answer[(selected + offset) % 4]);
            switch (_getch())
            {
                case 72: // Arrow Up
                    selected = (selected + 3) % 4;
                    confirmed = false;
                    break;
                case 80: // Arrow Down
                    selected = (selected + 1) % 4;
                    confirmed = false;
                    break;
                default:
                    break;
            }

            SetCursorPosition(3, questionEndLine + selected);
            printf(UNDERLINE_ON "%c: %s" UNDERLINE_OFF, 'A' + selected, question->Answer[(selected + offset) % 4]);
            continue;
        }

        if(answer == '\015') {
            if(confirmed) {
                int answerIndex = selected;
                answerIndex += offset;
                answerIndex = (answerIndex + 4) % 4;

                *outAnswer = (char)answerIndex;

                *outCorrect = answerIndex == 0;

                SetCursorPosition(3, questionEndLine + selected);

                if(*outCorrect)
                    SetColor(COLOR_FG_GREEN);
                else
                    SetColor(COLOR_FG_RED);

                printf(UNDERLINE_ON "%c: %s" UNDERLINE_OFF, 'A' + selected, question->Answer[(selected + offset) % 4]);
                ResetColor();

                while(_getch() != '\015');

                SetCursorPosition(0, questionEndLine + 10);
                return *outCorrect;
            }

            SetCursorPosition(3, questionEndLine + selected);
            SetColors(COLOR_FG_CYAN, COLOR_BG_DEFAULT);
            printf(UNDERLINE_ON "%c: %s" UNDERLINE_OFF, 'A' + selected, question->Answer[(selected + offset) % 4]);
            ResetColor();
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

        if(answer != 'A' && answer != 'B' && answer != 'C' && answer != 'D') {
            continue;
        }

        int answerIndex = answer - 'A';
        answerIndex += offset;
        answerIndex = (answerIndex + 4) % 4;

        *outAnswer = (char)answerIndex;

        *outCorrect = answerIndex == 0;

        SetCursorPosition(2, questionEndLine + (answer - 'A'));

        if(*outCorrect)
            SetColor(COLOR_FG_GREEN);
        else
            SetColor(COLOR_FG_RED);

        printf("%c: %s", answer, question->Answer[answerIndex]);
        ResetColor();

        while(_getch() != '\015');

        SetCursorPosition(0, questionEndLine + 10);
        return *outCorrect;
    }
}
