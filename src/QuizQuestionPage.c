#include "QuizQuestionPage.h"
#include "AnsiHelper.h"
#include "PageUtils.h"
#include "Settings.h"
#include "Utf8Symbols.h"
#include <time.h>

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define REWARD_BOX_WIDTH 18
#define REWARD_BOX_WIDTH_PLUS_BORDER (REWARD_BOX_WIDTH + 2)

#define ABILITY_AUDIENCE 0
#define ABILITY_5050 1
#define ABILITY_PHONE 2

extern Settings* LoadedSettings;

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

    double audienceVotes[4];
    bool blockedOptions[4];

    bool* abilities;

    bool abilitiesConfirm[3];
    bool abilitiesNow[3];

    int selectedQuestion;
    bool confirmed;

    bool previewMode;
} QuizQuestionPageData;

void ShowAudienceHelp(QuizQuestionPageData* data);
void Show5050Help(QuizQuestionPageData* data);
void ShowPhoneHelp(QuizQuestionPageData* data);

void SetCursorToRestingPlace(QuizQuestionPageData* data) {
    SetCursorPosition(0, data->answersEndY + 5);
}

void PrintSingleAnswerBlock(int beginX, int beginY, int ansWidth, int lineCount, char letter, bool color, int colorFg) {
    if(color) SetColor(colorFg);
    SetCursorPosition(beginX, beginY);

    const int totalMargin = 1 + 1; // Top + Bottom

    const int titleWidth = 12; // "Odpowiedź: X"
    const int startText = (ansWidth - titleWidth)/2 - 2;
    const int endText = startText + titleWidth + 1;

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
    if(selected == oldSelected) return;
    
    bool forceUpdate = oldSelected == INT_MIN;

    int beginX = 3;
    int beginY = data->questionContentEndY;
    int beginXOffset = data->ansWidth + 1;

    if(forceUpdate || selected == 0 || oldSelected == 0) 
        PrintSingleAnswerBlock(beginX, beginY, data->ansWidth, data->ansLineCountMaxAB, 'A', selected == 0, colorFg);
    if(forceUpdate || selected == 1 || oldSelected == 1) 
        PrintSingleAnswerBlock(beginX + beginXOffset, beginY, data->ansWidth, data->ansLineCountMaxAB, 'B', selected == 1, colorFg);

    beginY += data->ansLineCountMaxAB + 4;

    if(forceUpdate || selected == 2 || oldSelected == 2) 
        PrintSingleAnswerBlock(beginX, beginY, data->ansWidth, data->ansLineCountMaxCD, 'C', selected == 2, colorFg);
    if(forceUpdate || selected == 3 || oldSelected == 3) 
        PrintSingleAnswerBlock(beginX + beginXOffset, beginY, data->ansWidth, data->ansLineCountMaxCD, 'D', selected == 3, colorFg);
}

void PrintAnswersBlocksForce(QuizQuestionPageData* data, int selected, int colorFg) {
    PrintAnswersBlocks(data, selected, INT_MIN, colorFg);
}

void PrintAnswerContent(QuizQuestionPageData* data, int startX, int startY, int ansIndex, int height) {
    SetCursorPosition(startX, startY);
    PrintWrappedLine(data->question->Answer[ansIndex], data->ansWidthLimit, startX - 1, true);
    if(data->abilitiesNow[ABILITY_5050] && data->blockedOptions[ansIndex]) {
        SetCursorPosition(startX + data->ansWidthLimit - 11, startY + height);
        SetColor(LoadedSettings->WrongAnswerColor);
        printf("NIEPOPRAWNA");
        ResetColor();
    }
    else if(data->abilitiesNow[ABILITY_AUDIENCE]) { // Print Audience help
        SetCursorPosition(startX + data->ansWidthLimit - 4, startY + height);
        SetColor(LoadedSettings->SupportColor);
        printf("%2.1f%%", data->audienceVotes[ansIndex]);
        ResetColor();
    }
}

// Calculate the amount of lines needed to wrap the text
void CalculateQuizQuestionPageData(QuizQuestionPageData* data) {
    GetTerminalSize(&data->terminalWidth, &data->terminalHeight);

    if(data->terminalWidth % 2 == 0) data->terminalWidth--;

    const int questionEndLinePredefinedY = 3;
    data->questionContentEndY = GetWrappedLineCount(data->question->Content, data->terminalWidth - 16) + questionEndLinePredefinedY;

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

// Create a new QuizQuestionPageData object
QuizQuestionPageData* new_QuizQuestionPageData(Question* question, int number, int offset, bool* abilities) {
    QuizQuestionPageData* data = malloc(sizeof(QuizQuestionPageData));
    data->question = question;
    data->questionNumer = number;
    data->offset = offset;
    data->abilities = abilities;
    data->selectedQuestion = 0;
    data->confirmed = false;
    data->previewMode = false;

    data->abilitiesNow[ABILITY_AUDIENCE] = false;
    data->abilitiesNow[ABILITY_5050] = false;
    data->abilitiesNow[ABILITY_PHONE] = false;

    data->abilitiesConfirm[ABILITY_AUDIENCE] = false;
    data->abilitiesConfirm[ABILITY_5050] = false;
    data->abilitiesConfirm[ABILITY_PHONE] = false;

    // Generate random audience votes
    int votes[4] = {0, 0, 0, 0};
    int ansCount = 1000;
    
    votes[0] = (rand() % 200) + 400;
    votes[1] = (rand() % (1000 - votes[0]));
    votes[2] = (rand() % (1000 - votes[0] - votes[1]));
    votes[3] = 1000 - votes[0] - votes[1] - votes[2];

    for (int i = 0; i < 4; i++)
        data->audienceVotes[i] = ((double)votes[i] / (double)ansCount) * 100.0;

    // Block random answers
    data->blockedOptions[0] = false;
    data->blockedOptions[1] = false;
    data->blockedOptions[2] = false;
    data->blockedOptions[3] = false;

    for (int i = 0; i < 2; i++)
    {
        int randomIndex;
        do {
            randomIndex = (rand() % 3) + 1;
        }
        while (data->blockedOptions[randomIndex]);
        data->blockedOptions[randomIndex] = true;
    }

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
    const int questionCount = 10;
    for (int i = 0; i < questionCount; i++) {
        bool colorSet = false;
        SetCursorPosition(data->terminalWidth - REWARD_BOX_WIDTH, data->questionContentEndY + i);
        if(!data->previewMode && data->questionNumer == questionCount - i) {
            SetColor(LoadedSettings->SelectedAnswerColor);
            colorSet = true;
        }
        printf("%2d ", questionCount - i);

        if (LoadedSettings->FullUTF8Support)
            printf(TRIANGLE);
        else
            printf(">");

        printf(" ");
        switch (questionCount - (i + 1)) {
            case 0:
                if(!colorSet) SetColor(LoadedSettings->SupportColor);
                printf("      500 zł");  // save point 1
                break;

            case 1:
                printf("    1 000 zł");
                break;

            case 2:
                if(!colorSet) SetColor(LoadedSettings->SupportColor);
                printf("    4 000 zł");  // save point 2
                break;

            case 3:
                printf("   10 000 zł");
                break;

            case 4:
                printf("   25 000 zł"); 
                break;

            case 5:
                if(!colorSet) SetColor(LoadedSettings->SupportColor);
                printf("   50 000 zł");  // save point 3
                break;

            case 6:
                printf("  100 000 zł");
                break;

            case 7:
                printf("  250 000 zł");
                break;  

            case 8:
                printf("  500 000 zł");
                break;

            case 9:
                printf("1 000 000 zł");
                break;
        }

        ResetColor();
    }
}

void DrawStaticUI_QuestionContent(QuizQuestionPageData* data) {
    SetCursorPosition(3, 2);
    printf("Pytanie %2d: ", data->questionNumer);
    PrintWrappedLine(data->question->Content, data->terminalWidth - 16, 14, false);
}

void PrintAbilityText(QuizQuestionPageData* data, int answerId, const char* text, char key) {
    printf("                                                      ");
    printf(CSR_MOVE_LEFT(54));
    if(data->abilitiesNow[answerId]) {
        SetColor(LoadedSettings->SelectedAnswerColor);
        printf("%c) %s: Wykorzystano", key, text);
    }
    else if(data->abilities[answerId]) {
        SetColor(LoadedSettings->WrongAnswerColor);
        printf("%c) %s: Wykorzystano", key, text);
    }
    else if(data->abilitiesConfirm[answerId]) {
        SetColor(LoadedSettings->ConfirmedAnswerColor);
        printf("%c) %s: Potwierdź wykorzystanie (%c)", key, text, key);
    }
    else {
        SetColor(LoadedSettings->CorrectAnswerColor);
        printf("%c) %s: Dostępny", key, text);
    }
}

void DrawStaticUI_Abilities(QuizQuestionPageData* data) {
    SetCursorPosition(3, data->answersEndY + 1);
    
    PrintAbilityText(data, ABILITY_AUDIENCE, "Głos publiczności", '1');

    printf(CSR_MOVE_LEFT_0_DOWN1 CSR_MOVE_RIGHT(2));
    PrintAbilityText(data, ABILITY_5050, "50/50", '2');

    printf(CSR_MOVE_LEFT_0_DOWN1 CSR_MOVE_RIGHT(2));
    PrintAbilityText(data, ABILITY_PHONE, "Telefon do przyjaciela", '3');
    
    ResetColor();

    SetCursorToRestingPlace(data);
}

void DrawStaticUI_Answers(QuizQuestionPageData* data) {
    ResetCursor();

    if(!data->previewMode) {
        PrintAnswersBlocksForce(data, data->selectedQuestion, LoadedSettings->SelectedAnswerColor);
    }

    const int ansTextLeftPadding = 5;
    const int ansTextMiddlePadding = 5;

    int startY = data->questionContentEndY + 2;
    int startXOffset = data->ansWidthLimit + ansTextMiddlePadding;

    // Print Answer 1
    PrintAnswerContent(data, ansTextLeftPadding, startY, (0 + data->offset) % 4, data->ansLineCountMaxAB);
    // Print Answer 2
    PrintAnswerContent(data, ansTextLeftPadding + startXOffset, startY, (1 + data->offset) % 4, data->ansLineCountMaxAB);

    startY += data->ansLineCountMaxAB + 4;

    // Print Answer 3
    PrintAnswerContent(data, ansTextLeftPadding, startY, (2 + data->offset) % 4, data->ansLineCountMaxCD);
    // Print Answer 4
    PrintAnswerContent(data, ansTextLeftPadding + startXOffset, startY, (3 + data->offset) % 4, data->ansLineCountMaxCD);

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

    DrawStaticUI_QuestionContent(data);

    DrawStaticUI_Abilities(data);

    DrawStaticUI_Answers(data);

    SetCursorToRestingPlace(data);
}

void CheckToRedrawUI(time_t* lastCheckTime, QuizQuestionPageData* data) {
    time_t currentTime;
    time(&currentTime);

    if(difftime(currentTime, *lastCheckTime) < 1)
        return;

    *lastCheckTime = currentTime;

    int newTerminalX, newTerminalY;
    GetTerminalSize(&newTerminalX, &newTerminalY);
    if(newTerminalX % 2 == 0) newTerminalX--;

    if(newTerminalX == data->terminalWidth && newTerminalY == data->terminalHeight)
        return;

    // Resize and redraw UI
    CalculateQuizQuestionPageData(data);
    DrawStaticUI(data);
}

void UpdateAnswersBlocks(QuizQuestionPageData* data, int oldSelected) {
    data->confirmed = false;
    PrintAnswersBlocks(data, data->selectedQuestion, oldSelected, LoadedSettings->SelectedAnswerColor);
}

bool HandleAbilityButton(QuizQuestionPageData* data, int abilityId, int* abilityConfirmId) {
    // Allow to reopen help window
    if(data->abilitiesNow[abilityId]) {
        return false;
    }

    if(data->abilities[abilityId]) {
        return true;
    }

    if(!data->abilitiesConfirm[abilityId]) {
        *abilityConfirmId = abilityId;
        data->abilitiesConfirm[abilityId] = true;
        DrawStaticUI_Abilities(data);
        return true;
    }

    data->abilitiesNow[abilityId] = data->abilities[abilityId] = true;
    return false;
}

bool HandleKeyInput(QuizQuestionPageData* data, KeyInputType key, bool* outCorrect) {
    int oldSel = data->selectedQuestion;
    int abilityConfirmId = -1;

    switch (key)
    {
        case KEY_ARROW_UP:
            if(data->selectedQuestion == 2 || data->selectedQuestion == 3) {
                data->selectedQuestion -= 2;
                UpdateAnswersBlocks(data, oldSel);
            }
            break;
        case KEY_ARROW_DOWN:
            if(data->selectedQuestion == 0 || data->selectedQuestion == 1) {
                data->selectedQuestion += 2;
                UpdateAnswersBlocks(data, oldSel);
            }
            break;
        case KEY_ARROW_RIGHT:
            if(data->selectedQuestion == 0 || data->selectedQuestion == 2) {
                data->selectedQuestion += 1;
                UpdateAnswersBlocks(data, oldSel);
            }
            break;
        case KEY_ARROW_LEFT:
            if(data->selectedQuestion == 1 || data->selectedQuestion == 3) {
                data->selectedQuestion -= 1;
                UpdateAnswersBlocks(data, oldSel);
            }
            break;
        
        case KEY_ENTER:
            if(!data->confirmed) {
                PrintAnswersBlocksForce(data, data->selectedQuestion, LoadedSettings->ConfirmedAnswerColor);
                data->confirmed = true;
                break;
            }

            int answerIndex = (data->selectedQuestion + data->offset) % 4;

            *outCorrect = answerIndex == 0;

            if(*outCorrect) {
                PrintAnswersBlocksForce(data, data->selectedQuestion, LoadedSettings->CorrectAnswerColor);
            }
            else {
                PrintAnswersBlocksForce(data, data->selectedQuestion, LoadedSettings->WrongAnswerColor);
                if(LoadedSettings->ShowCorrectWhenWrong) {
                    PrintAnswersBlocks(data, (4 - data->offset) % 4, INT_MAX, LoadedSettings->CorrectAnswerColor);
                }
            }

            SetCursorToRestingPlace(data);
            WaitForEnter();

            free(data);
            return true;

        case KEY_1:
            if(HandleAbilityButton(data, ABILITY_AUDIENCE, &abilityConfirmId))
                break;

            ShowAudienceHelp(data);

            // Redraw UI
            DrawStaticUI(data);
            break;

        case KEY_2:
            if(HandleAbilityButton(data, ABILITY_5050, &abilityConfirmId))
                break;

            Show5050Help(data);

            // Redraw UI
            DrawStaticUI(data);
            break;

        case KEY_3:
            if(HandleAbilityButton(data, ABILITY_PHONE, &abilityConfirmId))
                break;

            ShowPhoneHelp(data);

            // Redraw UI
            DrawStaticUI(data);
            break;

        case KEY_R:
            // Recalculate and redraw UI
            CalculateQuizQuestionPageData(data);
            DrawStaticUI(data);
            break;

        default:
            return false;
    }

    for (int i = 0; i < 3; i++)
    {
        if(abilityConfirmId != i && data->abilitiesConfirm[i]) {
            data->abilitiesConfirm[i] = false;
            DrawStaticUI_Abilities(data);
        }
    }

    return false;
}

void PageEnter_QuizQuestion(Question* question, int number, bool* abilities, bool* outCorrect) { 
    int offset = rand() % 4;

    QuizQuestionPageData* data = new_QuizQuestionPageData(question, number, offset, abilities);

    DrawStaticUI(data);
    
    time_t lastCheckTime;
    time(&lastCheckTime);

    while (true)
    {
        KeyInputType key = HandleInteractions(false);

        if(HandleKeyInput(data, key, outCorrect))
            return;
    
        if(LoadedSettings->AutoResizeUI) {
            CheckToRedrawUI(&lastCheckTime, data);
        }
    }
}

void PageEnter_QuizQuestionPreview(Question* question) { 
    int offset = rand() % 4;

    bool abilities[3] = {true, true, true};

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
                // Recalculate and redraw UI
                CalculateQuizQuestionPageData(data);
                DrawStaticUI(data);
                break;

            case KEY_NONE:
                if(LoadedSettings->AutoResizeUI)
                    CheckToRedrawUI(&tmp, data);
                break;

            default:
                break;
        }
    }

    free(data);
    data = NULL;
}

void ShowAudienceHelp(QuizQuestionPageData* data) {
    const int windowWidth = 54;
    const int beginX = (data->terminalWidth - windowWidth) / 2;
    int beginY = 2;
    int windowHeight = 13;
    int segmentCount = 9;

    SetCursorPosition(beginX, beginY);

    PRINT_SINGLE_TOP_BORDER(windowWidth);

    for (int i = 1; i < windowHeight; i++)
        PrintGenericBorderEdges(beginX, windowWidth, beginY + i, SINGLE_VERTICAL_LINE, true);

    SetCursorPosition(beginX, beginY + windowHeight);
    PRINT_SINGLE_BOTTOM_BORDER(windowWidth);

    SetCursorPosition(beginX + 2, beginY + 1);
    PrintWrappedLine("Publiczność zagłosowała za odpowiedziami.", windowWidth - 4, beginX + 2, true);

    for (int j = 0; j < 4; j++)
    {
        SetCursorPosition(beginX + 2, beginY + windowHeight - 2 - (segmentCount / 3)*j);
        SetColor(LoadedSettings->SupportColor);

        printf(UNDERLINE_ON "%2d%%", 20*j);

        for (int i = 0; i < windowWidth - 8; i++)
            printf(" ");
        printf(UNDERLINE_OFF);
        ResetColor();
    }

    const int barWidth = 8;
    const int ansWidthPlusSep = barWidth + 3;
    const int barsOffset = 8;
    SetCursorPosition(beginX + barsOffset, beginY + windowHeight - 1);
    printf("A: %2.1f%%", data->audienceVotes[(0 + data->offset) % 4]);

    SetCursorPosition(beginX + barsOffset + ansWidthPlusSep, beginY + windowHeight - 1);
    printf("B: %2.1f%%", data->audienceVotes[(1 + data->offset) % 4]);

    SetCursorPosition(beginX + barsOffset + ansWidthPlusSep*2, beginY + windowHeight - 1);
    printf("C: %2.1f%%", data->audienceVotes[(2 + data->offset) % 4]);

    SetCursorPosition(beginX + barsOffset + ansWidthPlusSep*3, beginY + windowHeight - 1);
    printf("D: %2.1f%%", data->audienceVotes[(3 + data->offset) % 4]);

    double oneSegment = 0.6 / segmentCount;
    double oneSegmentSmall = oneSegment / 8.0;
    double drawnVotes[4] = {0, 0, 0, 0};
    for (int i = 0; i < 4; i++)
    {
        drawnVotes[i] = data->audienceVotes[i] / 100.0;
        if(drawnVotes[i] > 0.6) drawnVotes[i] = 0.6; // Cap graph at 60%
    }
    
    for (int i = 0; i < segmentCount; i++)
    {
        for (int id = 0; id < 4; id++)
        {
            SetCursorPosition(beginX + barsOffset + ansWidthPlusSep*id, beginY + windowHeight - 1 - i - 1);
            if(drawnVotes[(id + data->offset) % 4] >= oneSegment) {
                for(int j = 0; j < barWidth; j++) {
                    printf(BLOCK_8_8);
                }
                drawnVotes[(id + data->offset) % 4] -= oneSegment;
                continue;
            }

            if(!LoadedSettings->FullUTF8Support) {
                continue;
            }

            if(drawnVotes[(id + data->offset) % 4] > oneSegmentSmall) {
                int counter = 0;
                while ((drawnVotes[(id + data->offset) % 4] -= oneSegmentSmall) > 0) {
                    counter++;
                }

                for(int j = 0; j < barWidth; j++) {
                    printf(BLOCK_x_8(counter));
                }
                drawnVotes[(id + data->offset) % 4] = 0;
            }
        }
    }

    SetCursorToRestingPlace(data);
    WaitForKeys(ENTER, '1');
}

void Show5050Help(QuizQuestionPageData* data) {
    const int windowWidth = 49 + 4;
    const int beginX = (data->terminalWidth - windowWidth) / 2;
    int beginY = 3;
    int widnowHeight = 3;

    SetCursorPosition(beginX, beginY);
    PRINT_SINGLE_TOP_BORDER(windowWidth);

    for (int i = 1; i < widnowHeight; i++)
        PrintGenericBorderEdges(beginX, windowWidth, beginY + i, SINGLE_VERTICAL_LINE, true);

    SetCursorPosition(beginX, beginY + widnowHeight);
    PRINT_SINGLE_BOTTOM_BORDER(windowWidth);

    SetCursorPosition(beginX + 2, beginY + 1);
    PrintWrappedLine("Wykreśliłem dla ciebie 2 niepoprawne odpowiedzi.", windowWidth - 4, beginX + 2, true);

    SetCursorToRestingPlace(data);
    WaitForKeys(ENTER, '2');
}

void ShowPhoneHelp(QuizQuestionPageData* data) {
    const int windowWidth = 49 + 4;
    const int beginX = (data->terminalWidth - windowWidth) / 2;
    int beginY = 3;
    int widnowHeight = 3;

    SetCursorPosition(beginX, beginY);
    PRINT_SINGLE_TOP_BORDER(windowWidth);

    for (int i = 1; i < widnowHeight; i++)
        PrintGenericBorderEdges(beginX, windowWidth, beginY + i, SINGLE_VERTICAL_LINE, true);

    SetCursorPosition(beginX, beginY + widnowHeight);
    PRINT_SINGLE_BOTTOM_BORDER(windowWidth);

    SetCursorPosition(beginX + 2, beginY + 1);
    PrintWrappedLine("Niestety twój przyiaciel nie odbiera.", windowWidth - 4, beginX + 2, true);

    SetCursorToRestingPlace(data);
    WaitForKeys(ENTER, '3');
}
