#include "QuizQuestionPage.h"
#include "AnsiHelper.h"
#include "PageUtils.h"
#include "Settings.h"
#include "Utf8Symbols.h"
#include <time.h>

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define REWARD_BOX_WIDTH 18
#define REWARD_BOX_WIDTH_PLUS_BORDER (REWARD_BOX_WIDTH + 2)

extern Settings* LoadedSettings;

double* ShowAudienceHelp(int offset);

typedef struct
{
    Question* question;
    int questionNumer;
    int offset;

    int terminalWidth;
    int terminalHeight;

    int questionContentEndY;
    int answersEndY;

    int ansWidthLimit;
    int ansWidth;
    int ansLineCountMaxAB;
    int ansLineCountMaxCD;

    double* audienceVotes;
    bool* abilities;

    bool abilitiesNow[3];

    int selectedQuestion;
    bool confirmed;

    bool previewMode;
} QuizQuestionPageData;

void PrintSingleAnsBlock(int beginX, int beginY, int ansWidth, int lineCount, char letter, bool color, int colorFg) {
    if(color) SetColor(colorFg);
    SetCursorPosition(beginX, beginY);

    const int topMargin = 1;
    const int bottomMargin = 1;
    const int totalMargin = topMargin + bottomMargin;

    const int titleWidth = 14;
    const int startText = (ansWidth - titleWidth)/2 - 1;
    const int endText = startText + titleWidth - 1;

    printf(SINGLE_TOP_LEFT_CORNER);
    for (int i = 0; i < ansWidth - 2; i++) {
        if(i == startText)
            printf(SINGLE_BREAK_LEFT);
        else if(i == endText)
            printf(SINGLE_BREAK_RIGHT);
        else if(i > startText && i < endText)
            printf(CSR_MOVE_RIGHT_1); // Text goes here
        else
            printf(SINGLE_HORIZONTAL_LINE);
    }
    printf(SINGLE_TOP_RIGHT_CORNER);

    for (int i = 0; i < lineCount + totalMargin; i++)
        PrintGenericBorderEdges(beginX, ansWidth - 1, beginY + i + 1, SINGLE_VERTICAL_LINE, false);

    SetCursorPosition(beginX, beginY + lineCount + totalMargin + 1);
    PRINT_SINGLE_BOTTOM_BORDER(ansWidth);

    // Print text
    SetCursorPosition(beginX + startText + 2, beginY);

    SetColor(LoadedSettings->ConfirmedAnswerColor);
    printf("Odpowiedź: %c", letter);

    ResetColor();
}

void PrintAnswersBlocks(QuizQuestionPageData* data, int selected, int oldSelected, int colorFg) {
    bool forceUpdate = oldSelected == INT_MIN;
    if(!forceUpdate && selected == oldSelected && oldSelected != INT_MAX) return;

    int beginY = data->questionContentEndY;

    if(forceUpdate || selected == 0 || oldSelected == 0) 
        PrintSingleAnsBlock(3, beginY, data->ansWidth, data->ansLineCountMaxAB, 'A', selected == 0, colorFg);
    if(forceUpdate || selected == 1 || oldSelected == 1) 
        PrintSingleAnsBlock(3 + data->ansWidth + 1, beginY, data->ansWidth, data->ansLineCountMaxAB, 'B', selected == 1, colorFg);

    beginY += data->ansLineCountMaxAB + 4;

    if(forceUpdate || selected == 2 || oldSelected == 2) 
        PrintSingleAnsBlock(3, beginY, data->ansWidth, data->ansLineCountMaxCD, 'C', selected == 2, colorFg);
    if(forceUpdate || selected == 3 || oldSelected == 3) 
        PrintSingleAnsBlock(3 + data->ansWidth + 1, beginY, data->ansWidth, data->ansLineCountMaxCD, 'D', selected == 3, colorFg);
}

void PrintAnswersBlocksForce(QuizQuestionPageData* data, int selected, int colorFg) {
    PrintAnswersBlocks(data, selected, INT_MIN, colorFg);
}

void PrintAnsContent(QuizQuestionPageData* data, int startX, int startY, int ansIndex, int height) {
    SetCursorPosition(startX, startY);
    PrintWrappedLine(data->question->Answer[ansIndex], data->ansWidthLimit, startX - 1, true);
    if(data->audienceVotes != NULL) { // Print Audience help
        SetCursorPosition(startX + data->ansWidthLimit - 4, startY + height);
        printf("%2.1f%%", data->audienceVotes[ansIndex]);
    }
}

void CalculateQuizQuestionPageData(QuizQuestionPageData* data) {
    GetTerminalSize(&data->terminalWidth, &data->terminalHeight);

    if(data->terminalWidth % 2 == 0) data->terminalWidth--;

    const int questionEndLinePredefinedY = 3;
    data->questionContentEndY = GetWrappedLineCount(data->question->Content, data->terminalWidth - 15) + questionEndLinePredefinedY;

    const int marginsBetweenAnsers = 13;
    data->ansWidthLimit = (data->terminalWidth - marginsBetweenAnsers - REWARD_BOX_WIDTH_PLUS_BORDER) / 2;
    data->ansWidth = data->ansWidthLimit + 4;
    
    int ansLineCountA = GetWrappedLineCount(data->question->Answer[(0 + data->offset) % 4], data->ansWidthLimit);
    int ansLineCountB = GetWrappedLineCount(data->question->Answer[(1 + data->offset) % 4], data->ansWidthLimit);
    data->ansLineCountMaxAB = MAX(ansLineCountA, ansLineCountB);
    int ansLineCountC = GetWrappedLineCount(data->question->Answer[(2 + data->offset) % 4], data->ansWidthLimit);
    int ansLineCountD = GetWrappedLineCount(data->question->Answer[(3 + data->offset) % 4], data->ansWidthLimit);
    data->ansLineCountMaxCD = MAX(ansLineCountC, ansLineCountD);
    data->answersEndY = data->questionContentEndY + data->ansLineCountMaxAB + data->ansLineCountMaxCD + 8;
}

QuizQuestionPageData* new_QuizQuestionPageData(Question* question, int number, int offset, bool* abilities) {
    QuizQuestionPageData* data = malloc(sizeof(QuizQuestionPageData));
    data->question = question;
    data->questionNumer = number;
    data->offset = offset;
    data->audienceVotes = NULL;
    data->abilities = abilities;
    data->selectedQuestion = 0;
    data->confirmed = false;
    data->previewMode = false;

    data->abilitiesNow[0] = false;
    data->abilitiesNow[1] = false;
    data->abilitiesNow[2] = false;

    CalculateQuizQuestionPageData(data);

    return data;
}

void DrawStaticUI_Border_RewardBox(QuizQuestionPageData* data) {
    // Draw reward box
    SetCursorPosition(data->terminalWidth - REWARD_BOX_WIDTH - 2, data->questionContentEndY - 1);
    printf(TJUNCTION_DOWN_SINGLE);
    for (int i = 0; i < MAX(data->answersEndY - data->questionContentEndY, 10); i++) {
        SetCursorPosition(data->terminalWidth - REWARD_BOX_WIDTH - 2, data->questionContentEndY + i);
        printf(SINGLE_LIGHT_VERTICAL_LINE);
    }

    // Add connector for reward box
    SetCursorPosition(data->terminalWidth - REWARD_BOX_WIDTH - 2, data->answersEndY);
    printf(TJUNCTION_UP_SINGLE);
}

void DrawStaticUI_Border_Main(QuizQuestionPageData* data) {
    ResetCursor();

    PRINT_TOP_BORDER(data->terminalWidth);

    for (int i = 2; i < (data->answersEndY + 4); i++)
        PrintGenericBorderEdges(0, data->terminalWidth, i, VERTICAL_LINE, false);

    printf(CSR_MOVE_LEFT_0_DOWN1);
    PRINT_BOTTOM_BORDER(data->terminalWidth);
}

void DrawStaticUI_Border(QuizQuestionPageData* data) {
    ResetCursor();

    DrawStaticUI_Border_Main(data);

    // Question Content | Answers splitter
    SetCursorPosition(0, data->questionContentEndY - 1);
    PRINT_TJUNCTION_BORDER(data->terminalWidth);
    
    // Answers | Abilities spliter
    SetCursorPosition(0, data->answersEndY);
    PRINT_TJUNCTION_BORDER(data->terminalWidth);

    DrawStaticUI_Border_RewardBox(data);
}

void DrawStatusUI_RewardBoxContent(QuizQuestionPageData* data) {
    for (int i = 0; i < 10; i++) {
        SetCursorPosition(data->terminalWidth - REWARD_BOX_WIDTH, data->questionContentEndY + i);
        if(!data->previewMode && data->questionNumer == 10 - i) {
            SetColor(LoadedSettings->SelectedAnswerColor);
        }
        printf("%2d ", 10 - i);

        if (LoadedSettings->FullUTF8Support)
            printf(TRIANGLE);
        else
            printf(">");

        printf(" ");
        switch (9 - i) {
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
                // printf("  250 000 zł");
                printf("1 000 000 zł");
                break;

            // case 10:
            //     printf("  500 000 zł");
            //     break;

            // case 11:
            //     printf("1 000 000 zł");
            //     break;
        }

        ResetColor();
    }
}

void DrawStaticUI_Other(QuizQuestionPageData* data) {
    ResetCursor();

    SetCursorPosition(3, 2);
    printf("Pytanie %2d: ", data->questionNumer);
    PrintWrappedLine(data->question->Content, data->terminalWidth - 16, 14, false);
    printf("\n");

    SetCursorPosition(3, data->answersEndY + 1);
    
    if(!data->abilities[0]) {
        SetColor(LoadedSettings->CorrectAnswerColor);
        printf("X: Głos publiczności");
    }
    else {
        SetColor(LoadedSettings->WrongAnswerColor);
        printf("X: Głos publiczności niedostępny");
    }
    ResetColor();

    printf(CSR_MOVE_LEFT_0_DOWN1 CSR_MOVE_RIGHT(2));
    if(!data->abilities[1])
        printf("Y: Help2");
    else
        printf("Y: Already used");

    printf(CSR_MOVE_LEFT_0_DOWN1 CSR_MOVE_RIGHT(2));
    if(!data->abilities[2])
        printf("Z: Help3");
    else
        printf("Z: Already used");
}

void DrawStaticUI_Answers(QuizQuestionPageData* data) {
    ResetCursor();

    if(!data->previewMode) PrintAnswersBlocksForce(data, data->selectedQuestion, LoadedSettings->SelectedAnswerColor);

    const int ansTextLeftPadding = 5;
    const int ansTextMiddlePadding = 5;

    // Print Answer 1
    PrintAnsContent(data, ansTextLeftPadding, data->questionContentEndY + 2, (0 + data->offset) % 4, data->ansLineCountMaxAB);
    // Print Answer 2
    PrintAnsContent(data, ansTextLeftPadding + data->ansWidthLimit + ansTextMiddlePadding, data->questionContentEndY + 2,  (1 + data->offset) % 4, data->ansLineCountMaxAB);

    // Print Answer 3
    PrintAnsContent(data, ansTextLeftPadding, data->questionContentEndY + data->ansLineCountMaxAB + 6,  (2 + data->offset) % 4, data->ansLineCountMaxCD);
    // Print Answer 4
    PrintAnsContent(data, ansTextLeftPadding + data->ansWidthLimit + ansTextMiddlePadding, data->questionContentEndY + data->ansLineCountMaxAB + 6, (3 + data->offset) % 4, data->ansLineCountMaxCD);

    if(data->previewMode) {
        PrintAnswersBlocks(data, (0 - data->offset + 4) % 4, INT_MAX, LoadedSettings->CorrectAnswerColor);
        PrintAnswersBlocks(data, (1 - data->offset + 4) % 4, INT_MAX, LoadedSettings->WrongAnswerColor);
        PrintAnswersBlocks(data, (2 - data->offset + 4) % 4, INT_MAX, LoadedSettings->WrongAnswerColor);
        PrintAnswersBlocks(data, (3 - data->offset + 4) % 4, INT_MAX, LoadedSettings->WrongAnswerColor);
    }
}

void DrawStaticUI(QuizQuestionPageData* data) {
    ClearScreen();
    DrawStaticUI_Border(data);

    DrawStatusUI_RewardBoxContent(data);

    DrawStaticUI_Other(data);

    DrawStaticUI_Answers(data);
}

void CheckToRedrawUI(time_t* lastCheckTime, QuizQuestionPageData* data, double timeLimit) {
    time_t currentTime;
    time(&currentTime);

    if(difftime(currentTime, *lastCheckTime) < timeLimit)
        return;

    if(timeLimit > 0) {
        *lastCheckTime = currentTime;
    }

    int newTerminalX, newTerminalY;
    GetTerminalSize(&newTerminalX, &newTerminalY);
    if(newTerminalX % 2 == 0) newTerminalX--;

    if(newTerminalX == data->terminalWidth && newTerminalY == data->terminalHeight)
        return;

    // Resize and redraw UI
    CalculateQuizQuestionPageData(data);
    DrawStaticUI(data);
}

bool HandleKeyInput(QuizQuestionPageData* data, KeyInputType key, bool* outCorrect, char* outAnswer) {
    int oldSel = data->selectedQuestion;

    switch (key)
    {
        case KEY_ARROW_UP:
            if(data->selectedQuestion == 2 || data->selectedQuestion == 3) {
                data->selectedQuestion -= 2;
                data->confirmed = false;
                PrintAnswersBlocks(data, data->selectedQuestion, oldSel, LoadedSettings->SelectedAnswerColor);
            }
            break;
        case KEY_ARROW_DOWN:
            if(data->selectedQuestion == 0 || data->selectedQuestion == 1) {
                data->selectedQuestion += 2;
                data->confirmed = false;
                PrintAnswersBlocks(data, data->selectedQuestion, oldSel, LoadedSettings->SelectedAnswerColor);
            }
            break;
        case KEY_ARROW_RIGHT:
            if(data->selectedQuestion == 0 || data->selectedQuestion == 2) {
                data->selectedQuestion += 1;
                data->confirmed = false;
                PrintAnswersBlocks(data, data->selectedQuestion, oldSel, LoadedSettings->SelectedAnswerColor);
            }
            break;
        case KEY_ARROW_LEFT:
            if(data->selectedQuestion == 1 || data->selectedQuestion == 3) {
                data->selectedQuestion -= 1;
                data->confirmed = false;
                PrintAnswersBlocks(data, data->selectedQuestion, oldSel, LoadedSettings->SelectedAnswerColor);
            }
            break;
        
        case KEY_ENTER:
            if(data->confirmed) {
                int answerIndex = data->selectedQuestion;
                answerIndex += data->offset;
                answerIndex = (answerIndex + 4) % 4;

                *outAnswer = (char)answerIndex;
                *outCorrect = answerIndex == 0;

                int fgColor = *outCorrect ? LoadedSettings->CorrectAnswerColor : LoadedSettings->WrongAnswerColor;
                PrintAnswersBlocksForce(data, data->selectedQuestion, fgColor);

                ResetColor();

                WaitForEnter();

                SetCursorPosition(0, data->answersEndY + 10);
                free(data->audienceVotes);
                free(data);
                return true;
            }

            PrintAnswersBlocksForce(data, data->selectedQuestion, LoadedSettings->ConfirmedAnswerColor);
            data->confirmed = true;
            break;

        case KEY_X:
            if(data->abilities[0]) break;
            data->abilitiesNow[0] = data->abilities[0] = true;

            data->audienceVotes = ShowAudienceHelp(data->offset);

            // Redraw UI
            DrawStaticUI(data);
            break;

        case KEY_Y:
            if(data->abilities[1]) break;
            data->abilitiesNow[1] = data->abilities[1] = true;

            SetCursorPosition(3, data->answersEndY + 6);
            printf("Y: %s\n", data->question->Help);
            break;

        case KEY_Z:
            if(data->abilities[2]) break;
            data->abilitiesNow[2] = data->abilities[2] = true;

            SetCursorPosition(3, data->answersEndY + 7);
            printf("Z: %s\n", data->question->Help);
            break;

        case KEY_R:
            CheckToRedrawUI(NULL, data, 0);
            break;

        default:
            break;
    }

    return false;
}

void PageEnter_QuizQuestion(Question* question, int number, bool* abilities, bool* outCorrect, char* outAnswer) { 
    int offset = rand() % 4;

    QuizQuestionPageData* data = new_QuizQuestionPageData(question, number, offset, abilities);

    DrawStaticUI(data);
    
    time_t lastCheckTime;
    time(&lastCheckTime);

    while (true)
    {
        KeyInputType key = HandleInteractions(false);

        if(HandleKeyInput(data, key, outCorrect, outAnswer))
            return;
    
        if(LoadedSettings->AutoResizeUI) {
            CheckToRedrawUI(&lastCheckTime, data, 1);
        }
    }
}

void PageEnter_QuizQuestionPreview(Question* question) { 
    int offset = rand() % 4;

    bool abilities[3] = {false, false, false};

    QuizQuestionPageData* data = new_QuizQuestionPageData(question, question->Id, offset, abilities);
    data->previewMode = true;

    DrawStaticUI(data);

    time_t tmp;
    time(&tmp);

    bool continueLoop = true;
    while(continueLoop) {
        switch (HandleInteractions(false))
        {
            case KEY_ENTER:
                continueLoop = false;
                break;

            case KEY_R:
                CheckToRedrawUI(NULL, data, 0);
                break;

            case KEY_NONE:
                if(LoadedSettings->AutoResizeUI)
                    CheckToRedrawUI(&tmp, data, 1);
                break;

            default:
                break;
        }
    }

    free(data);
    data = NULL;
}

double* ShowAudienceHelp(int offset) {
    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

    const int windowWidth = 60;
    const int beginX = (terminalWidth - windowWidth) / 2;
    int beginY = 3;
    int widnowHeight = 25;
    int segmentCount = 20;

    if(terminalHeight < widnowHeight + beginY) {
        beginY = terminalHeight - widnowHeight - 1;
        if(beginY <= 0) {
            beginY += 3;
            widnowHeight = terminalHeight - 5;
            segmentCount -= 5;
        }
    }

    SetCursorPosition(beginX, beginY);

    PRINT_SINGLE_TOP_BORDER(windowWidth);

    for (int i = 1; i < widnowHeight; i++)
        PrintGenericBorderEdges(beginX, windowWidth, beginY + i, SINGLE_VERTICAL_LINE, true);

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
