#include "UIManager.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <math.h>
#include "QuizManager.h"
#include "AnsiHelper.h"
#include "PageUtils.h"
#include "Settings.h"
#include "SettingsPage.h"

extern Settings* LoadedSettings;

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

#define SINGLE_LIGHT_VERTICAL_LINE "%c%c%c", 0xE2, 0x94, 0x82 /*'┃'*/

#define BLOCK_x_8(x) "%c%c%c", 0xE2, 0x96, (0x80+x) /*'▁'*/
#define BLOCK_1_8 "%c%c%c", 0xE2, 0x96, 0x81 /*'▁'*/
#define BLOCK_2_8 "%c%c%c", 0xE2, 0x96, 0x82 /*'▂'*/
#define BLOCK_3_8 "%c%c%c", 0xE2, 0x96, 0x83 /*'▃'*/
#define BLOCK_4_8 "%c%c%c", 0xE2, 0x96, 0x84 /*'▄'*/
#define BLOCK_5_8 "%c%c%c", 0xE2, 0x96, 0x85 /*'▅'*/
#define BLOCK_6_8 "%c%c%c", 0xE2, 0x96, 0x86 /*'▆'*/
#define BLOCK_7_8 "%c%c%c", 0xE2, 0x96, 0x87 /*'▇'*/
#define BLOCK_8_8 "%c%c%c", 0xE2, 0x96, 0x88 /*'█'*/

#define TRIANGLE "%c%c%c", 0xE2, 0x96, 0xB6 /*'▶'*/

#define DASHED_HORIZONTAL_LINE "%c%c%c", 0xE2, 0x95, 0x8C /*'╌'*/

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

#define PRINT_SINGLE_BOTTOM_BORDER(width) {\
    printf(SINGLE_BOTTOM_LEFT_CORNER); \
    for (int _i = 0; _i < width - 2; _i++) \
        printf(SINGLE_HORIZONTAL_LINE); \
    printf(SINGLE_BOTTOM_RIGHT_CORNER); printf("\n"); }


#define MAX(x, y) ((x) > (y) ? (x) : (y))

void PrintMainMenu()
{
    ResetColor();
    ClearScreen();
    printf("Witaj w Programisterach!\n");
    printf("1. Rozpocznij quiz\n");
    printf("2. Dodaj pytanie\n");
    printf("3. Tablica wyników\n");
    printf("4. Ustawienia\n");
    printf("Q. Wyjdź\n");
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
            case '4':
                EnterSettings();
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
            ExitOnCtrlC();
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
            WaitForEnter();
            break;
        }
    }

    DestroyQuiz(quiz);

    UILoop_MainMenu();
}

void PrintSingleAnsBlock(int beginY, int ansWidth, int lineCount, char letter, bool color, int colorFg, int beginX) {
    if(color) SetColor(colorFg);
    SetCursorPosition(beginX, beginY);

    int titleWidth = 14;
    int startText = (ansWidth - titleWidth)/2 - 1;
    int endText = startText + titleWidth - 1;
    printf(SINGLE_TOP_LEFT_CORNER);
    for (int i = 0; i < ansWidth - 2; i++) {
        if(i == startText) printf(SINGLE_BREAK_LEFT);
        else if(i == endText) printf(SINGLE_BREAK_RIGHT);
        else if(i > startText && i < endText) printf(CSR_MOVE_RIGHT_1);
        else printf(SINGLE_HORIZONTAL_LINE);
    }
    printf(SINGLE_TOP_RIGHT_CORNER);

    for (int i = 0; i < lineCount + 2; i++) {
        SetCursorPosition(beginX, beginY + i + 1);
        printf(SINGLE_VERTICAL_LINE);
        SetCursorPosition(beginX + ansWidth - 1, beginY + i + 1);
        printf(SINGLE_VERTICAL_LINE);
    }

    SetCursorPosition(beginX, beginY + lineCount + 3);
    PRINT_SINGLE_BOTTOM_BORDER(ansWidth);
    if(color) ResetColor();

    // Print letter
    SetCursorPosition(beginX + (ansWidth - titleWidth)/2 + 1, beginY);

    SetColor(LoadedSettings->ConfirmedAnswerColor);
    printf("Odpowiedź: %c", letter);

    ResetColor();
}

void PrintDoubleAnsBlock(int beginY, int ansWidth, int lineCount, char letter, int color, int colorFg) {
    PrintSingleAnsBlock(beginY, ansWidth, lineCount, letter, color == 1, colorFg, 3);
    PrintSingleAnsBlock(beginY, ansWidth, lineCount, letter + 1, color == 2, colorFg, 3 + ansWidth + 1);
}

void PrintQuadAnsBlock(int beginY, int ansWidth, int lineCount1, int lineCount2, int color, int colorFg) {
    PrintDoubleAnsBlock(beginY, ansWidth, lineCount1, 'A', color + 1, colorFg);
    PrintDoubleAnsBlock(beginY + lineCount1 + 4, ansWidth, lineCount2, 'C', color - 1, colorFg);
}

void PrintDoubleAnsBlockDelta(int beginY, int ansWidth, int lineCount, char letter, int color, int colorFg, int oldColor) {
    if(color == oldColor) return;
    if(color == 1 || oldColor == 1) PrintSingleAnsBlock(beginY, ansWidth, lineCount, letter, color == 1, colorFg, 3);
    if(color == 2 || oldColor == 2) PrintSingleAnsBlock(beginY, ansWidth, lineCount, letter + 1, color == 2, colorFg, 3 + ansWidth + 1);
}

void PrintQuadAnsBlockDelta(int beginY, int ansWidth, int lineCount1, int lineCount2, int selected, int colorFg, int oldSelected) {
    if(selected == oldSelected) return;
    if(selected == 0 || selected == 1 || oldSelected == 0 || oldSelected == 1)
        PrintDoubleAnsBlockDelta(beginY, ansWidth, lineCount1, 'A', selected + 1, colorFg, oldSelected + 1);
    if(selected == 2 || selected == 3 || oldSelected == 2 || oldSelected == 3)
        PrintDoubleAnsBlockDelta(beginY + lineCount1 + 4, ansWidth, lineCount2, 'C', selected - 1, colorFg, oldSelected - 1);
}

double* ShowAudienceHelp(int offset) {
    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

    const int windowWidth = 60;
    const int beginX = (terminalWidth - windowWidth) / 2;
    int beginY = 3;
    int widnowHeight = 25;
    int segmentCount = 20;

    if(terminalHeight < 30) {
        beginY = terminalHeight - widnowHeight - 1;
        if(beginY <= 0) {
            beginY += 3;
            widnowHeight = terminalHeight - 5;
            segmentCount -= 5;
        }
    }

    SetCursorPosition(beginX, beginY);

    printf(SINGLE_TOP_LEFT_CORNER); 
    for (int _i = 0; _i < windowWidth - 2; _i++) 
        printf(SINGLE_HORIZONTAL_LINE); 
    printf(SINGLE_TOP_RIGHT_CORNER);
    printf("\n");

    for (int i = 1; i < widnowHeight; i++) {
        SetCursorPosition(beginX, beginY + i);
        printf(SINGLE_VERTICAL_LINE);
        for (int j = 0; j < windowWidth - 2; j++)
        {
            printf(" ");
        }
        printf(SINGLE_VERTICAL_LINE);
    }

    SetCursorPosition(beginX, beginY + widnowHeight);
    PRINT_SINGLE_BOTTOM_BORDER(windowWidth);

    SetCursorPosition(beginX + 2, beginY + 1);
    PrintWrappedLine("Publiczność zagłosowała za odpowiedziami.", windowWidth - 4, beginX + 2, true);

    for (int j = 0; j < 6; j++)
    {
        SetCursorPosition(beginX + 3, beginY + widnowHeight - 2 - (segmentCount / 5)*j);
        SetColor(LoadedSettings->SupportColor);

        printf(UNDERLINE_ON "%2d%%", 20*j);

        for (int i = 0; i < windowWidth - 8; i++)
            printf(" ");
        printf(UNDERLINE_OFF);
        ResetColor();
    }

    int votes[4] = {0, 0, 0, 0};
    const int ansCount = 10000;
    const int correctAnsChance = 50;
    for (int i = 0; i < ansCount; i++)
    {
        double value = rand() % 100;
        if(value < correctAnsChance) {
            votes[0]++;
        }
        else {
            double incorrectAnsChance = (100 - correctAnsChance) / 3;
            value -= correctAnsChance;
            if(value < incorrectAnsChance) {
                votes[1]++;
            }
            else {
                value -= incorrectAnsChance;
                if(value < incorrectAnsChance) {
                    votes[2]++;
                }
                else {
                    votes[3]++;
                }
            }
        }
    }

    const int barWidth = 9;
    const int ansWidthPlusSep = barWidth + 3;
    const int barsOffset = 8;
    SetCursorPosition(beginX + barsOffset, beginY + widnowHeight - 1);
    printf("A: %2.1f%%", votes[(0 + offset) % 4] / (ansCount / 100.0));

    SetCursorPosition(beginX + barsOffset + ansWidthPlusSep, beginY + widnowHeight - 1);
    printf("B: %2.1f%%", votes[(1 + offset) % 4] / (ansCount / 100.0));

    SetCursorPosition(beginX + barsOffset + ansWidthPlusSep*2, beginY + widnowHeight - 1);
    printf("C: %2.1f%%", votes[(2 + offset) % 4] / (ansCount / 100.0));

    SetCursorPosition(beginX + barsOffset + ansWidthPlusSep*3, beginY + widnowHeight - 1);
    printf("D: %2.1f%%", votes[(3 + offset) % 4] / (ansCount / 100.0));

    double oneSegment = 1.0 / segmentCount;
    double oneSegmentSmall = oneSegment / 8.0;
    double drawnVotes[4] = {0, 0, 0, 0};
    for (int i = 0; i < 4; i++)
    {
        drawnVotes[i] = (double)votes[i] / (double)ansCount;
    }
    
    for (int i = 0; i < segmentCount; i++)
    {
        for (int id = 0; id < 4; id++)
        {
            SetCursorPosition(beginX + barsOffset + ansWidthPlusSep*id, beginY + widnowHeight - 1 - i - 1);
            if(drawnVotes[(id + offset) % 4] >= oneSegment) {
                for(int j = 0; j < barWidth; j++) printf(BLOCK_8_8);
                drawnVotes[(id + offset) % 4] -= oneSegment;
            }
            else if(LoadedSettings->FullUTF8Support && drawnVotes[(id + offset) % 4] > 0) {
                int counter = 0;
                while ((drawnVotes[(id + offset) % 4] -= oneSegmentSmall) > 0) counter++;

                if(counter > 0) {
                    for(int j = 0; j < barWidth; j++) printf(BLOCK_x_8(counter));
                }
                drawnVotes[(id + offset) % 4] = 0;
            }
        }
    }

    WaitForEnter();

    double* tor = malloc(4 * sizeof(double));
    for (int i = 0; i < 4; i++)
    {
        tor[i] = ((double)votes[i] / (double)ansCount) * 100.0;
    }

    return tor;
}

bool UILoop_QuizQuestion(Question* question, int number, bool* abilities, bool* outCorrect, char* outAnswer) { 
    int offset = rand() % 4;

    int questionEndLine;
    int endAnsLine;
    int tmp;
    int terminalX, terminalY;
    bool abilitiesNow[3] = {false, false, false};
    double* audienceVotes = NULL;

    ui_reset:
    GetTerminalSize(&terminalX, &terminalY);
    if(terminalX % 2 == 0) terminalX--;

    ClearScreen();

    printf(TOP_LEFT_CORNER); 
    for (int _i = 0; _i < terminalX - 2; _i++) 
        printf(HORIZONTAL_LINE); 
    printf(TOP_RIGHT_CORNER);

    printf("\n  Pytanie %2d: ", number);
    PrintWrappedLine(question->Content, terminalX - 15, 14, false);
    printf("\n");
    PRINT_MIDDLE_BORDER(terminalX);
    GetCursorPosition(&tmp, &questionEndLine);

    for (int i = 2; i < questionEndLine - 1; i++)
        PRINT_VERTICAL_LINES(terminalX, i);

    
    int ansWidthLimit = (terminalX - 13 - 20) / 2;
    int ansLineCountA = GetWrappedLineCount(question->Answer[(0 + offset) % 4], ansWidthLimit);
    int ansLineCountB = GetWrappedLineCount(question->Answer[(1 + offset) % 4], ansWidthLimit);
    int ansLineCountMaxAB = MAX(ansLineCountA, ansLineCountB);
    int ansLineCountC = GetWrappedLineCount(question->Answer[(2 + offset) % 4], ansWidthLimit);
    int ansLineCountD = GetWrappedLineCount(question->Answer[(3 + offset) % 4], ansWidthLimit);
    int ansLineCountMaxCD = MAX(ansLineCountC, ansLineCountD);
    endAnsLine = questionEndLine + ansLineCountMaxAB + ansLineCountMaxCD + 8;

    const int ansTextLeftPadding = 5;
    const int ansTextMiddlePadding = 5;

    int ansWidth = ansWidthLimit + 4;

    int selected = 0;
    bool confirmed = false;

    // Draw reward box
    const int rewardBoxWidth = 18;
    SetCursorPosition(terminalX - rewardBoxWidth - 2, questionEndLine - 1);
    printf("%c%c%c", 0xE2, 0x95, 0xA4);
    for (int i = 0; i < MAX(endAnsLine - questionEndLine, 10); i++)
    {
        SetCursorPosition(terminalX - rewardBoxWidth - 2, questionEndLine + i);
        printf(SINGLE_LIGHT_VERTICAL_LINE);
        
        if(i < 10) {
            SetCursorPosition(terminalX - rewardBoxWidth, questionEndLine + i);
            if(number == 10 - i) {
                SetColor(LoadedSettings->SelectedAnswerColor);
            }
            printf("%2d ", 10 - i);

            if (LoadedSettings->FullUTF8Support)
                printf(TRIANGLE);
            else
                printf(">");

            printf(" ");
            switch (9 - i)
            {
                case 0:
                    printf("      500 zł");
                    break;

                case 1:
                    printf("    1 000 zł");
                    break;

                case 2:
                    printf("    2 000 zł");
                    break;

                case 3:
                    printf("    5 000 zł");
                    break;

                case 4:
                    printf("   10 000 zł");
                    break;

                case 5:
                    printf("   20 000 zł");
                    break;

                case 6:
                    //printf("   40 000 zł");
                    printf("   50 000 zł");
                    break;

                case 7:
                    // printf("   75 000 zł");
                    printf("  100 000 zł");
                    break;  

                case 8:
                    // printf("  125 000 zł");
                    printf("  250 000 zł");
                    break;

                case 9:
                    printf("1 000 000 zł");
                    // printf("  250 000 zł");
                    break;

                case 10:
                    printf("  500 000 zł");
                    break;

                case 11:
                    printf("1 000 000 zł");
                    break;
            
            default:
                break;
            }
            ResetColor();
        }
    }

    SetCursorPosition(0, questionEndLine);
    PrintQuadAnsBlock(questionEndLine, ansWidth, ansLineCountMaxAB, ansLineCountMaxCD, selected, LoadedSettings->SelectedAnswerColor);

    // Print Answer 1
    SetCursorPosition(ansTextLeftPadding, questionEndLine + 2);
    PrintWrappedLine(question->Answer[(0 + offset) % 4], ansWidthLimit, ansTextLeftPadding - 1, true);
    if(audienceVotes != NULL) { // Print Audience help
        SetCursorPosition(ansTextLeftPadding + ansWidthLimit - 4, questionEndLine + ansLineCountMaxAB + 2);
        printf("%2.1f%%", audienceVotes[(0 + offset) % 4]);
    }
    // Print Answer 2
    SetCursorPosition(ansTextLeftPadding + ansWidthLimit + ansTextMiddlePadding, questionEndLine + 2);
    PrintWrappedLine(question->Answer[(1 + offset) % 4], ansWidthLimit, ansTextLeftPadding + ansWidthLimit + ansTextMiddlePadding - 1, true);
    if(audienceVotes != NULL) { // Print Audience help
        SetCursorPosition(ansTextLeftPadding + ansWidthLimit + ansTextMiddlePadding + ansWidthLimit - 4, questionEndLine + ansLineCountMaxAB + 2);
        printf("%2.1f%%", audienceVotes[(1 + offset) % 4]);
    }

    // Print Answer 3
    SetCursorPosition(ansTextLeftPadding, questionEndLine + ansLineCountMaxAB + 6);
    PrintWrappedLine(question->Answer[(2 + offset) % 4], ansWidthLimit, ansTextLeftPadding - 1, true);
    if(audienceVotes != NULL) { // Print Audience help
        SetCursorPosition(ansTextLeftPadding + ansWidthLimit - 4, questionEndLine + ansLineCountMaxAB + 6 + ansLineCountMaxCD);
        printf("%2.1f%%", audienceVotes[(2 + offset) % 4]);
    }
    // Print Answer 4
    SetCursorPosition(ansTextLeftPadding + ansWidthLimit + ansTextMiddlePadding, questionEndLine + ansLineCountMaxAB + 6);
    PrintWrappedLine(question->Answer[(3 + offset) % 4], ansWidthLimit, ansTextLeftPadding + ansWidthLimit + ansTextMiddlePadding - 1, true);
    if(audienceVotes != NULL) { // Print Audience help
        SetCursorPosition(ansTextLeftPadding + ansWidthLimit + ansTextMiddlePadding + ansWidthLimit - 4, questionEndLine + ansLineCountMaxAB + 6 + ansLineCountMaxCD);
        printf("%2.1f%%", audienceVotes[(3 + offset) % 4]);
    }


    for (int i = questionEndLine; i < endAnsLine; i++)
        PRINT_VERTICAL_LINES(terminalX, i);
    
    SetCursorPosition(0, endAnsLine);
    PRINT_MIDDLE_BORDER(terminalX);

    // Add connector for reward box
    SetCursorPosition(terminalX - rewardBoxWidth - 2, endAnsLine);
    printf("%c%c%c", 0xE2, 0x95, 0xA7);

    SetCursorPosition(0, endAnsLine + 1);
    
    if(!abilities[0]) {
        SetColor(LoadedSettings->CorrectAnswerColor);
        printf("  X: Głos publiczności\n");
    }
    else {
        SetColor(LoadedSettings->WrongAnswerColor);
        printf("  X: Głos publiczności niedostępny\n");
    }
    ResetColor();

    PRINT_VERTICAL_LINES(terminalX, endAnsLine + 1);

    if(!abilities[1])
        printf("\n  Y: Help2\n");
    else
        printf("\n  Y: Already used\n");

    PRINT_VERTICAL_LINES(terminalX, endAnsLine + 2);

    if(!abilities[2])
        printf("\n  Z: Help3\n");
    else
        printf("\n  Z: Already used\n");

    PRINT_VERTICAL_LINES(terminalX, endAnsLine + 3);

    printf(CSR_MOVE_LEFT_0_DOWN1);

    printf(BOTTOM_LEFT_CORNER); 
    for (int _i = 0; _i < terminalX - 2; _i++) 
        printf(HORIZONTAL_LINE); 
    printf(BOTTOM_RIGHT_CORNER);

    printf(CSR_MOVE_LEFT_0_DOWN(2));
    
    while (true)
    {
        HideCursor();
        SetCursorPosition(2, questionEndLine + 7);

        int oldSel = selected;

        switch (HandleInteractions(false))
        {
            case KEY_ARROW_UP:
                if(selected == 2 || selected == 3) {
                    selected -= 2;
                    confirmed = false;
                    PrintQuadAnsBlockDelta(questionEndLine, ansWidth, ansLineCountMaxAB, ansLineCountMaxCD, selected, LoadedSettings->SelectedAnswerColor, oldSel);
                }
                break;
            case KEY_ARROW_DOWN:
                if(selected == 0 || selected == 1) {
                    selected += 2;
                    confirmed = false;
                    PrintQuadAnsBlockDelta(questionEndLine, ansWidth, ansLineCountMaxAB, ansLineCountMaxCD, selected, LoadedSettings->SelectedAnswerColor, oldSel);
                }
                break;
            case KEY_ARROW_RIGHT:
                if(selected == 0 || selected == 2) {
                    selected += 1;
                    confirmed = false;
                    PrintQuadAnsBlockDelta(questionEndLine, ansWidth, ansLineCountMaxAB, ansLineCountMaxCD, selected, LoadedSettings->SelectedAnswerColor, oldSel);
                }
                break;
            case KEY_ARROW_LEFT:
                if(selected == 1 || selected == 3) {
                    selected -= 1;
                    confirmed = false;
                    PrintQuadAnsBlockDelta(questionEndLine, ansWidth, ansLineCountMaxAB, ansLineCountMaxCD, selected, LoadedSettings->SelectedAnswerColor, oldSel);
                }
                break;
            
            case KEY_ENTER:
                if(confirmed) {
                    int answerIndex = selected;
                    answerIndex += offset;
                    answerIndex = (answerIndex + 4) % 4;

                    *outAnswer = (char)answerIndex;
                    *outCorrect = answerIndex == 0;

                    int fgColor = *outCorrect ? LoadedSettings->CorrectAnswerColor : LoadedSettings->WrongAnswerColor;
                    PrintQuadAnsBlock(questionEndLine, ansWidth, ansLineCountMaxAB, ansLineCountMaxCD, selected, fgColor);

                    ResetColor();

                    WaitForEnter();

                    SetCursorPosition(0, questionEndLine + 10);
                    free(audienceVotes);
                    return *outCorrect;
                }

                PrintQuadAnsBlock(questionEndLine, ansWidth, ansLineCountMaxAB, ansLineCountMaxCD, selected, LoadedSettings->ConfirmedAnswerColor);
                confirmed = true;
                break;

            case KEY_X:
                if(abilities[0]) break;
                abilitiesNow[0] = abilities[0] = true;

                audienceVotes = ShowAudienceHelp(offset);
                goto ui_reset;

            case KEY_Y:
                if(abilities[1]) break;
                abilitiesNow[1] = abilities[1] = true;

                SetCursorPosition(3, questionEndLine + 6);
                printf("Y: %s\n", question->Help);
                break;

            case KEY_Z:
                if(abilities[2]) break;
                abilitiesNow[2] = abilities[2] = true;

                SetCursorPosition(3, questionEndLine + 7);
                printf("Z: %s\n", question->Help);
                break;

            default:
                break;
        }
    }
}
