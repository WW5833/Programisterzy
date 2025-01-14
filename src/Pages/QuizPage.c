#include "QuizPage.h"
#include <stdbool.h>
#include "../QuizManager.h"
#include <stdio.h>
#include <stdlib.h>
#include "../IOHelper/IOHelper.h"
#include "QuizQuestionPage.h"
#include "../AnsiHelper.h"
#include "../Settings.h"
#include "../RGBColors.h"
#include "../Popup.h"
#include "../Utf8Symbols.h"
#include <string.h>
#include "../TextHelper.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y))

extern Settings LoadedSettings;
extern int LatestTerminalWidth, LatestTerminalHeight;

static void CalculateQuizPageSizeRequirements();

bool SetRewardColor(int rewardId, int questionNumber, int safe, QuizQuestionResult result) {
    if(result == QQR_Correct) {
        if(rewardId == 9) {
            SetColorRGBPreset(LoadedSettings.CorrectAnswerColor, false);
            return true;
        }
    }
    else if(result == QQR_Wrong) {
        if(rewardId == safe) {
            SetColorRGBPreset(LoadedSettings.CorrectAnswerColor, false);
            return true;
        }
    }
    else if(result == QQR_Forfeit) {
        if(rewardId == questionNumber - 1) {
            SetColorRGBPreset(LoadedSettings.CorrectAnswerColor, false);
            return true;
        }
    }

    return false;
}

void DrawpEndGameUIRightSide(int questionNumber, int safe, QuizQuestionResult result, int beginX, int beginY, int partSplit) {
    SetCursorPosition(beginX + partSplit, beginY + 1 - 1);
    printf(SINGLE_BREAK_BOTTOM);
    SetCursorPosition(beginX + partSplit + 16, beginY + 1 - 1);
    printf(SINGLE_BREAK_BOTTOM);
    for (int j = -1; j < 10; j++)
    {
        SetCursorPosition(beginX + partSplit, beginY + 1 + (9 - j));

        ResetColor();
        printf(SINGLE_VERTICAL_LINE);
        printf(" ");

        if(SetRewardColor(j, questionNumber, safe, result)) {
            printf("Twoja wygrana ");
            if (LoadedSettings.FullUTF8Support)
                printf(TRIANGLE);
            else
                printf(">");
        }
        else {
            printf(CSR_MOVE_RIGHT(14));
            printf(SINGLE_VERTICAL_LINE);
        }

        printf(" ");

        printf("%13s", GetRewardText(j));
    }
    ResetColor();
    SetCursorPosition(beginX + partSplit, beginY + 1 + 11);
    printf(SINGLE_BREAK_TOP);
    SetCursorPosition(beginX + partSplit + 16, beginY + 1 + 11);
    printf(SINGLE_BREAK_TOP);
}

void DrawEndGameUI(int questionNumber, int safe, QuizQuestionResult result) {
    const int width = 60;
    const int height = 13;

    int beginX, beginY;
    DrawEmptyPopup(width, height, &beginX, &beginY);

    const int partSplit = width - 32;
    DrawpEndGameUIRightSide(questionNumber, safe, result, beginX, beginY, partSplit);

    char* textPtr = NULL;
    int lines = 0;
    switch (result)
    {
        case QQR_Correct:
            SetColorRGBPreset(LoadedSettings.CorrectAnswerColor, false);

            SetCursorPosition(beginX + 2, beginY + 1 + ((height - 2 - 2) / 2) - 1);
            PrintWrappedLine("Gratulacje!", partSplit - 3, beginX + 1, true);
            lines++;

            textPtr = "Wygrałeś 1 000 000zł";
            break;

        case QQR_Wrong:
            SetColorRGBPreset(LoadedSettings.WrongAnswerColor, false);
            textPtr = "Niestety, nie udało Ci się wygrać głównej nagrody";
            break;

        case QQR_Forfeit:
            SetColorRGBPreset(LoadedSettings.SupportColor, false);
            textPtr = "Zrezygnowałeś z gry";
            break;
    }

    lines += GetWrappedLineCount(textPtr, partSplit - 3);
    SetCursorPosition(beginX + 2, beginY + 1 + ((height - 2 - lines) / 2));
    PrintWrappedLine(textPtr, partSplit - 3, beginX + 1, true);
}

void PageEnter_Quiz()
{
    HideCursor();

    ClearScreen();

    CalculateQuizPageSizeRequirements();

    QuestionListHeader* questions = GenerateQuiz();
    if(questions == NULL) return;

    QuizQuestionAbilityStatus abilitiesStatus[3] = {QQAS_Avaialable, QQAS_Avaialable, QQAS_Avaialable};
    QuizQuestionResult correct;
    QuestionListItem* current = questions->head;
    int i;
    for(i = 0; i < 10; i++) {

        for (int j = 0; j < 3; j++)
        {
            if(abilitiesStatus[j] == QQAS_Selected) {
                abilitiesStatus[j] = QQAS_Avaialable;
            }
            else if(abilitiesStatus[j] == QQAS_Active) {
                abilitiesStatus[j] = QQAS_Unavailable;
            }
        }

        PageEnter_QuizQuestion(current->data, i + 1, &abilitiesStatus[0], &correct);
        if(correct != QQR_Correct) break;
        current = current->next;
    }

    int safe = -1;
    if(i > 5) safe = 5;
    else if(i > 2) safe = 2;
    else if(i > 0) safe = 0;

    DrawEndGameUI(i, safe, correct);

    ResetColor();

    ListDestroy(questions, false);

    WaitForKeys(ENTER);
}

int QuizPageMinimumWidth = -1;
int QuizPageMinimumHeight = -1;

static int GetMaxWordLength(const char* line)
{
    int max = 0;
    const char* lastSpace = line;
    while (*line != '\0')
    {
        if (*line == ' ')
        {
            max = MAX(max, GetCharCount(lastSpace, line));
            lastSpace = line;
        }

        line++;
    }

    max = MAX(max, GetCharCount(lastSpace, line));

    return max;
}

void CalculateQuizPageSizeRequirements() {
    const int rewardBoxWidth = 18 + 1;
    const int rewardBoxHeight = 10;
    const int staticWidthContent = 14 + 2;
    const int staticWidthAnswer = 4 + 5 + 3 + rewardBoxWidth + 1;
    const int staticHeightAnswer = (1 + 1 + 1 + 1)*2;
    const int staticHeight = 1 + 1 + 1 + 3 + 1;

    const int absMinWidth = 57;

    int maxLengthWordAnswer = 0;
    int maxLengthWordContent = 0;

    int maxLineCountAnswer = 0;
    int maxLineCountContent = 0;

    QuestionListHeader* list = GetQuestionList();

    QuestionListItem* current = list->head;
    while (current != NULL)
    {
        int lengthWordContent = GetMaxWordLength(current->data->Content);
        maxLengthWordContent = MAX(maxLengthWordContent, lengthWordContent);

        for (int i = 0; i < 4; i++)
        {
            int lengthWordAnswer = GetMaxWordLength(current->data->Answer[i]);
            maxLengthWordAnswer = MAX(maxLengthWordAnswer, lengthWordAnswer);
        }

        current = current->next;
    }


    const int width = MAX(absMinWidth, MAX(staticWidthContent + maxLengthWordContent, staticWidthAnswer + maxLengthWordAnswer*2));
    const int minContentWidth = width - staticWidthContent;
    const int minAnswerWidth = (width - staticWidthAnswer)/2;

    current = list->head;
    while (current != NULL)
    {
        int lineCountContent = GetWrappedLineCount(current->data->Content, minContentWidth);
        maxLineCountContent = MAX(maxLineCountContent, lineCountContent);

        for (int i = 0; i < 4; i++)
        {
            int lineCountAnswer = GetWrappedLineCount(current->data->Answer[i], minAnswerWidth);
            maxLineCountAnswer = MAX(maxLineCountAnswer, lineCountAnswer);
        }

        current = current->next;
    }

    int height = staticHeight + maxLineCountContent + MAX((staticHeightAnswer + maxLineCountAnswer*2), rewardBoxHeight);

    QuizPageMinimumWidth = width;
    QuizPageMinimumHeight = height;
}
