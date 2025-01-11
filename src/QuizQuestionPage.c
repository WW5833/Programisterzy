#include "QuizQuestionPage.h"
#include "AnsiHelper.h"
#include "PageUtils.h"
#include "Settings.h"
#include "Utf8Symbols.h"
#include <time.h>
#include "IOHelper.h"
#include "RGBColors.h"
#include "Popup.h"
#include "TextHelper.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define REWARD_BOX_WIDTH 18
#define REWARD_BOX_WIDTH_PLUS_BORDER (REWARD_BOX_WIDTH + 2)

#define ABILITY_AUDIENCE 0
#define ABILITY_5050 1
#define ABILITY_PHONE 2

typedef enum {
    CFW_Question,
    CFW_Audience,
    CFW_5050,
    CFW_Phone,
    CFW_Exit,
} CurrentFocusedVindow;

typedef enum {
    PMA_None,
    PMA_AnswerConfirmation,
    PMA_AbilityActivation,
} PendingMouseAction;

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
    int phoneCallVote[4];

    QuizQuestionResult* outResult;
    QuizQuestionAbilityStatus* abilities;
    int mouseSelectedAbility;

    CurrentFocusedVindow focusedWindow;

    int selectedQuestion;
    bool confirmed;
    PendingMouseAction pendingAction;

    bool previewMode;
} QuizQuestionPageData;

void ShowAudienceHelp(QuizQuestionPageData* data);
void Show5050Help(QuizQuestionPageData* data);
void ShowPhoneHelp(QuizQuestionPageData* data);
bool ShowExitConfirmationWindow(QuizQuestionPageData* data);

void SetCursorToRestingPlace(QuizQuestionPageData* data) {
    SetCursorPosition(0, data->answersEndY + 5);
}

void PrintSingleAnswerBlock(int beginX, int beginY, int ansWidth, int lineCount, char letter, bool color, int colorFg) {
    if(color) SetColorRGBPreset(colorFg, false);
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

    SetColorRGBPreset(LoadedSettings->ConfirmedAnswerColor, false);
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
    if(data->abilities[ABILITY_5050] == QQAS_Active && data->blockedOptions[ansIndex]) {
        SetCursorPosition(startX + data->ansWidthLimit - 11, startY + height);
        SetColorRGBPreset(LoadedSettings->WrongAnswerColor, false);
        printf("NIEPOPRAWNA");
        ResetColor();
    }
    else if(data->abilities[ABILITY_AUDIENCE] == QQAS_Active) { // Print Audience help
        SetCursorPosition(startX + data->ansWidthLimit - 4, startY + height);
        SetColorRGBPreset(LoadedSettings->SupportColor, false);
        printf("%2.1f%%", data->audienceVotes[ansIndex]);
        ResetColor();
    }
    if(data->abilities[ABILITY_PHONE] == QQAS_Active && data->phoneCallVote[ansIndex] != 0) {
        SetCursorPosition(startX, startY + height);
        if(data->phoneCallVote[ansIndex] == 2) {
            SetColorRGBPreset(LoadedSettings->ConfirmedAnswerColor, false);
            printf("NIEPOPRAWNA");
        }
        else {
            SetColorRGBPreset(LoadedSettings->CorrectAnswerColor, false);
            printf("POPRAWNA ?");
        }
        ResetColor();
    }
}

extern int LatestTerminalWidth, LatestTerminalHeight;

// Calculate the amount of lines needed to wrap the text
void CalculateQuizQuestionPageData(QuizQuestionPageData* data, bool calculateTerminalSize) {
    if(calculateTerminalSize) {
        data->terminalWidth = LatestTerminalWidth;
        data->terminalHeight = LatestTerminalHeight;
    }

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

void ini_QuizQuestionPageData(QuizQuestionPageData* data, Question* question, int number, int offset, QuizQuestionAbilityStatus* abilities, QuizQuestionResult* outResult) {
    data->question = question;
    data->questionNumer = number;
    data->offset = offset;
    data->abilities = abilities;
    data->outResult = outResult;
    data->selectedQuestion = 0;
    data->confirmed = false;
    data->pendingAction = PMA_None;
    data->previewMode = false;
    data->mouseSelectedAbility = -1;
    data->focusedWindow = CFW_Question;

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

    data->phoneCallVote[0] = 0;
    data->phoneCallVote[1] = 0;
    data->phoneCallVote[2] = 0;
    data->phoneCallVote[3] = 0;

    int phoneVotePercentage = rand() % 100;

    if(phoneVotePercentage < 30){
        data->phoneCallVote[phoneVotePercentage / 10 + 1] = 2;
    }
    else if(phoneVotePercentage < 40){
        data->phoneCallVote[rand() % 3 + 1] = 1;
    }
    else{
        data->phoneCallVote[0] = 1;
    }

    CalculateQuizQuestionPageData(data, true);
}

void DrawStaticUI_Border_RewardBox(QuizQuestionPageData* data) {
    // Draw reward box
    SetCursorPosition(data->terminalWidth - REWARD_BOX_WIDTH - 2, data->questionContentEndY - 1);
    printf(TJUNCTION_DOWN_SINGLE);
    for (int i = 0; i < MAX(data->answersEndY - data->questionContentEndY, 10); i++) {
        SetCursorPosition(data->terminalWidth - REWARD_BOX_WIDTH - 2, data->questionContentEndY + i);
        printf(SINGLE_VERTICAL_LINE);
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

const char* GetRewardText(int rewardId) {
    switch (rewardId) {
        case -1: return "0 zł";
        case 0:  return "500 zł";
        case 1:  return "1 000 zł";
        case 2:  return "4 000 zł";
        case 3:  return "10 000 zł";
        case 4:  return "25 000 zł"; 
        case 5:  return "50 000 zł";
        case 6:  return "100 000 zł";
        case 7:  return "250 000 zł";
        case 8:  return "500 000 zł";
        case 9:  return "1 000 000 zł";
        default: return "";
    }
}

int GetRewardColor(QuizQuestionPageData* data, int rewardId) {
    int questionNum = data->questionNumer - 1;
    if(rewardId == questionNum) {
        return LoadedSettings->SelectedAnswerColor;
    }

    if(rewardId == 0 || rewardId == 2 || rewardId == 5) {
        if(rewardId < questionNum) {
            return LoadedSettings->CorrectAnswerColor;
        }

        return LoadedSettings->SupportColor;
    }

    return 0;
}

void DrawStatusUI_RewardBoxContent(QuizQuestionPageData* data) {
    const int questionCount = 10;
    for (int i = 0; i < questionCount; i++) {
        int rewardId = questionCount - (i + 1);
        SetCursorPosition(data->terminalWidth - REWARD_BOX_WIDTH, data->questionContentEndY + i);
        int color = GetRewardColor(data, rewardId);
        if(!data->previewMode && color != 0) SetColorRGBPreset(color, false);
        
        printf("%2d ", questionCount - i);

        if (LoadedSettings->FullUTF8Support)
            printf(TRIANGLE);
        else
            printf(">");

        printf(" ");

        printf("%13s", GetRewardText(rewardId));
        ResetColor();
    }
}

void DrawStaticUI_QuestionContent(QuizQuestionPageData* data) {
    SetCursorPosition(3, 2);
    printf("Pytanie %2d: ", data->questionNumer);
    PrintWrappedLine(data->question->Content, data->terminalWidth - 16, 14, false);
}

void PrintAbilityText(QuizQuestionPageData* data, int abilityId, const char* text, char key) {
    printf("                                                      ");
    printf(CSR_MOVE_LEFT(54));
    int status = data->abilities[abilityId];
    int color = 0;

    if(data->mouseSelectedAbility == abilityId) {
        color = LoadedSettings->SelectedAnswerColor;
    }

    switch (status)
    {
        case QQAS_Unavailable:
            SetColorRGBPreset(LoadedSettings->WrongAnswerColor, false);
            printf("%c) %s: Wykorzystano", key, text);
            break;
        
        case QQAS_Active:
            if(color == 0) SetColorRGBPreset(LoadedSettings->SupportColor, false);
            else SetColorRGBPreset(color, false);

            printf("%c) %s: Pokaż", key, text);
            break;

        case QQAS_Selected:
            if(color == 0) SetColorRGBPreset(LoadedSettings->ConfirmedAnswerColor, false);
            else SetColorRGBPreset(color, false);

            printf("%c) %s: Potwierdź wykorzystanie (%c)", key, text, key);
            break;

        case QQAS_Avaialable:
            if(color == 0) SetColorRGBPreset(LoadedSettings->CorrectAnswerColor, false);
            else SetColorRGBPreset(color, false);

            printf("%c) %s: Dostępny", key, text);
            break;
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

void UpdateAnswersBlocks(QuizQuestionPageData* data, int oldSelected) {
    data->confirmed = false;
    PrintAnswersBlocks(data, data->selectedQuestion, oldSelected, LoadedSettings->SelectedAnswerColor);
}

void ResetAbilityConfirm(QuizQuestionPageData* data, int selectedAbilityId) {
    for (int i = 0; i < 3; i++)
    {
        if(selectedAbilityId == i) continue;
        if(data->abilities[i] == QQAS_Selected) {
            data->abilities[i] = QQAS_Avaialable;
            DrawStaticUI_Abilities(data);
        }
    }
}

bool HandleAbilityButton(QuizQuestionPageData* data, int abilityId, int* abilityConfirmId) {
    // Allow to reopen help window
    if(data->abilities[abilityId] == QQAS_Active) {
        return false;
    }

    if(data->abilities[abilityId] == QQAS_Unavailable) {
        return true;
    }

    if(data->abilities[abilityId] != QQAS_Selected) {
        *abilityConfirmId = abilityId;
        data->abilities[abilityId] = QQAS_Selected;
        DrawStaticUI_Abilities(data);
        return true;
    }

    data->abilities[abilityId] = QQAS_Active;
    return false;
}

void RemoveMouseHandlers() {
    UnsetMouseHandler();
}

bool HandleAnswerConfirmation(QuizQuestionPageData* data, bool eventCalled) {
    if(!data->confirmed) {
        PrintAnswersBlocksForce(data, data->selectedQuestion, LoadedSettings->ConfirmedAnswerColor);
        data->confirmed = true;
        return false;
    }

    if(eventCalled) {
        data->pendingAction = PMA_AnswerConfirmation;
        return true;
    }

    int answerIndex = (data->selectedQuestion + data->offset) % 4;

    if(answerIndex == 0) {
        *data->outResult = QQR_Correct;
    }
    else {
        *data->outResult = QQR_Wrong;
    }

    if(*data->outResult == QQR_Correct) {
        PrintAnswersBlocksForce(data, data->selectedQuestion, LoadedSettings->CorrectAnswerColor);
    }
    else {
        PrintAnswersBlocksForce(data, data->selectedQuestion, LoadedSettings->WrongAnswerColor);
        if(LoadedSettings->ShowCorrectWhenWrong) {
            PrintAnswersBlocks(data, (4 - data->offset) % 4, INT_MAX, LoadedSettings->CorrectAnswerColor);
        }
    }

    RemoveMouseHandlers();
    EnableMouseInput(true);

    SetCursorToRestingPlace(data);
    WaitForKeys(ENTER, ESC, ANY_MOUSE_BUTTON);
    EnableMouseInput(false);
    return true;
}

bool HandleKeyInput(QuizQuestionPageData* data, KeyInputType key) {
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
            if(!HandleAnswerConfirmation(data, false)) {
                break;
            }

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
            CalculateQuizQuestionPageData(data, true);
            DrawStaticUI(data);
            break;

        case KEY_ESCAPE:
            if(ShowExitConfirmationWindow(data)) {
                *data->outResult = QQR_Forfeit;
                return true;
            }

            DrawStaticUI(data);
            break;

        default:
            return false;
    }

    ResetAbilityConfirm(data, abilityConfirmId);

    return false;
}

void OnConsoleResize(int newWidth, int newHeight, void* data) {
    QuizQuestionPageData* pageData = (QuizQuestionPageData*)data;
    pageData->terminalWidth = newWidth;
    pageData->terminalHeight = newHeight;

    CalculateQuizQuestionPageData(pageData, false);
    DrawStaticUI(pageData);
}

bool SelectAnswerBasedOnMousePosition(QuizQuestionPageData* data, int x, int y) {
    const int startX = data->questionContentEndY - 1;
    const int endX = data->answersEndY - 2;
    const int centerY = startX + data->ansLineCountMaxAB + 4;

    if(y < startX || y > endX) {
        return false;
    }

    const int leftX = 2;
    const int centerX = leftX + data->ansWidth;
    const int rightX = centerX + data->ansWidth;

    if(x < leftX || x > rightX || x == centerX) {
        return false;
    }

    int oldSel = data->selectedQuestion;
    int newSel = 0;

    if(y < centerY) {
        newSel = 0;
    }
    else {
        newSel = 2;
    }

    if(x > centerX) {
        newSel++;
    }

    if(newSel != oldSel) {
        data->selectedQuestion = newSel;
        ResetAbilityConfirm(data, -1);
        UpdateAnswersBlocks(data, oldSel);
    }

    return true;
}

bool SelectAbilityBasedOnMousePosition(QuizQuestionPageData* data, int y) {
    bool result = false;
    int oldSelected = data->mouseSelectedAbility;
    data->mouseSelectedAbility = -1;

    const int startY = data->answersEndY;
    const int endY = startY + 3;

    if(y < startY || y >= endY) {
        goto ret;
    }

    // if(x < 3 || x > 3 + 20) {
    //     goto ret;
    // }

    int abilityId = y - startY;

    if(abilityId == -1) {
        goto ret;
    }

    data->mouseSelectedAbility = abilityId;
    result = true;

    ret:

    if(oldSelected == data->mouseSelectedAbility) {
        return result;
    }

    DrawStaticUI_Abilities(data);

    return result;
}

void OnQuizQuestionPageMouseMove(int x, int y, void* data) {
    QuizQuestionPageData* pageData = (QuizQuestionPageData*)data;

    if(pageData->focusedWindow != CFW_Question) {
        return;
    }

    SelectAnswerBasedOnMousePosition(pageData, x, y);
    SelectAbilityBasedOnMousePosition(data, y);
}

void HandleMouseClickForAnswer(QuizQuestionPageData* data, int x, int y) {
    if(!SelectAnswerBasedOnMousePosition(data, x, y)) {
        return;
    }

    HandleAnswerConfirmation(data, true);
}

void HandleMouseClickForAbilities(QuizQuestionPageData* data, int y) {
    if(!SelectAbilityBasedOnMousePosition(data, y)) {
        return;
    }

    int abilityConfirmId = -1;
    if(HandleAbilityButton(data, data->mouseSelectedAbility, &abilityConfirmId)) {
        ResetAbilityConfirm(data, abilityConfirmId);
        return;
    }

    data->pendingAction = PMA_AbilityActivation;
}

void OnQuizQuestionPageMouseClick(int button, int x, int y, void* data) {
    QuizQuestionPageData* pageData = (QuizQuestionPageData*)data;

    if((button & MOUSE_LEFT_BUTTON) == 0) {
        return;
    }

    if(pageData->focusedWindow == CFW_Exit) {
        return;
    }

    if(pageData->focusedWindow == CFW_Question) {
        HandleMouseClickForAnswer(pageData, x, y);
        HandleMouseClickForAbilities(pageData, y);
    }

    // Maybe add option to exit from abilities windows but that whould require awaiting for mouse click in WaitForKeys
}

void OnQuizQuestionPageDoubleMouseClick(int button, int x, int y, void* data) {
    OnQuizQuestionPageMouseClick(button, x, y, data);
}

void PageEnter_QuizQuestion(Question* question, int number, QuizQuestionAbilityStatus* abilities, QuizQuestionResult* outResult) { 
    int offset = rand() % 4;

    QuizQuestionPageData data;
    ini_QuizQuestionPageData(&data, question, number, offset, abilities, outResult);

    DrawStaticUI(&data);

    SetResizeHandler(OnConsoleResize, &data);
    SetMouseHandler(OnQuizQuestionPageMouseClick, OnQuizQuestionPageDoubleMouseClick, NULL, OnQuizQuestionPageMouseMove, &data);

    while (true)
    {
        KeyInputType key = HandleInteractions(false);

        if(data.pendingAction == PMA_AnswerConfirmation) {
            data.pendingAction = PMA_None;
            HandleAnswerConfirmation(&data, false);
            break;
        } else if(data.pendingAction == PMA_AbilityActivation) {  
            data.pendingAction = PMA_None; 
            switch (data.mouseSelectedAbility)
            {
                case ABILITY_AUDIENCE:
                    ShowAudienceHelp(&data);
                    break;
                case ABILITY_5050:
                    Show5050Help(&data);
                    break;
                case ABILITY_PHONE:
                    ShowPhoneHelp(&data);
                    break;
            }

            DrawStaticUI(&data);
        }

        if(HandleKeyInput(&data, key)) {
            break;
        }
    }

    UnsetResizeHandler();
    RemoveMouseHandlers();
}

void PageEnter_QuizQuestionPreview(Question* question) { 
    int offset = rand() % 4;

    QuizQuestionAbilityStatus abilities[3] = {QQAS_Unavailable, QQAS_Unavailable, QQAS_Unavailable};
    QuizQuestionResult result;

    QuizQuestionPageData data;
    ini_QuizQuestionPageData(&data, question, question->Id, offset, abilities, &result);
    data.previewMode = true;

    DrawStaticUI(&data);

    SetResizeHandler(OnConsoleResize, &data);

    bool continueLoop = true;
    while(continueLoop) {
        switch (HandleInteractions(false))
        {
            case KEY_R:
                // Recalculate and redraw UI
                CalculateQuizQuestionPageData(&data, true);
                DrawStaticUI(&data);
                break;

            case KEY_ENTER:
            case KEY_ESCAPE:
                continueLoop = false;
                break;

            default:
                break;
        }
    }

    UnsetResizeHandler();
}

void ShowAudienceHelp(QuizQuestionPageData* data) {
    data->focusedWindow = CFW_Audience;

    const int windowWidth = 54;
    int windowHeight = 13;
    int segmentCount = 9;

    int beginX, beginY;
    DrawEmptyPopup(windowWidth, windowHeight + 1, &beginX, &beginY);

    SetCursorPosition(beginX + 2, beginY + 1);
    PrintWrappedLine("Publiczność zagłosowała za odpowiedziami.", windowWidth - 4, beginX + 2, true);

    for (int j = 0; j < 4; j++)
    {
        SetCursorPosition(beginX + 2, beginY + windowHeight - 2 - (segmentCount / 3)*j);
        SetColorRGBPreset(LoadedSettings->SupportColor, false);

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
    WaitForKeys(ENTER, '1', ESC, ANY_MOUSE_BUTTON);

    data->focusedWindow = CFW_Question;
}

void Show5050Help(QuizQuestionPageData* data) {
    data->focusedWindow = CFW_5050;

    ShowAlertPopupKeys("Wykreśliłem dla ciebie 2 niepoprawne odpowiedzi.", 48 + 4, ENTER, '2', ESC, ANY_MOUSE_BUTTON);

    data->focusedWindow = CFW_Question;
}

static const int PhoneHelpCorrectMessagesLength = 6;
static const char* PhoneHelpCorrectMessages[] = {
    "Wydaje mi się, że dpowiedź %c będzie poprawna.",
    "Jestem dosyć pewien, że odpowiedź %c jest poprawna.",
    "Jestem prawie pewien, że odpowiedź %c jest poprawna.",
    "Na twoim miejscu chyba zaznaczyłbym odpowiedź %c.",
    "Odpowiedź %c wydaje się dosyć prawdopodobna.",
    "Odpowiedź %c dobrze wygląda."
};

static const int PhoneHelpCorrectWrongLength = 5;
static const char* PhoneHelpWrongMessages[] = {
    "Wiem że odpowiedź %c jest na pewno niepoprawna.",
    "Moim zdaniem odpowiedź %c jest niepoprawna.",
    "Odpowiedź %c na pewno tutaj nie pasuje.",
    "Na twoim miejscu nie zaznaczałbym odpowiedzi %c.",
    "Odpowiedź %c wygląda dosyć fałszywie."
};

void ShowPhoneHelp(QuizQuestionPageData* data) {
    data->focusedWindow = CFW_Phone;
    bool ansIsCorrect = true;
    int i;
    for (i = 0; i < 4; i++) {
        if(data->phoneCallVote[i] == 2) {
            ansIsCorrect = false;
            break;
        }        
    }
    
    char* message;
    if(ansIsCorrect) {
        message = PhoneHelpCorrectMessages[rand() % PhoneHelpCorrectMessagesLength];
    } else {
        message = PhoneHelpWrongMessages[rand() % PhoneHelpCorrectWrongLength];
    }

    char buffor[128];
    sprintf(buffor, message, 'A' + ((4 + i - data->offset) % 4)); 

    const int popUpWidth = 44;
    int popUpHeight = 3;
    popUpHeight += GetWrappedLineCount(buffor, popUpWidth - 4);
    int beginX, beginY;
    DrawEmptyPopup(popUpWidth, popUpHeight + 2, &beginX, &beginY);

    SetCursorPosition(beginX + 2, beginY + 1);
    PrintWrappedLine("Telefon do Przyjaciela:", popUpWidth - 4, beginX + 2, true);

    SetCursorPosition(beginX + 2, beginY + 3);
    PrintWrappedLine(buffor, popUpWidth - 4, beginX + 2, true);

    SetCursorToRestingPlace(data);
    WaitForKeys(ENTER, '3', ESC, ANY_MOUSE_BUTTON);

    data->focusedWindow = CFW_Question;
}

bool ShowExitConfirmationWindow(QuizQuestionPageData* data) {
    data->focusedWindow = CFW_Exit;

    if(ShowConfirmationPopup(
        "Czy na pewno chcesz zakończyć grę?",
        "TAK",
        "NIE",
        34 + 4
        ))
    {
        data->focusedWindow = CFW_Question;
        return true;
    }

    data->focusedWindow = CFW_Question;
    return false;
}
