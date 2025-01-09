#include "QuizPage.h"
#include <stdbool.h>
#include "QuizManager.h"
#include <stdio.h>
#include <stdlib.h>
#include "PageUtils.h"
#include "QuizQuestionPage.h"
#include "AnsiHelper.h"
#include "Settings.h"
#include "RGBColors.h"

extern Settings* LoadedSettings;
extern int LatestTerminalWidth, LatestTerminalHeight;

void PageEnter_Quiz()
{
    HideCursor();

    ClearScreen();

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

    SetCursorPosition(0, LatestTerminalHeight - 1);

    char textBuffer[100];
    switch (correct)
    {
        case QQR_Correct:
            SetColorRGBPreset(LoadedSettings->CorrectAnswerColor, false);
            printf("Gratulacje! Wygrałeś 1 000 000zł\n");
            break;

        case QQR_Wrong:
            SetColorRGBPreset(LoadedSettings->WrongAnswerColor, false);
            sprintf(textBuffer, "Niestety, nie udało Ci się wygrać głównej nagrody ale dostajesz %s, ostatnią bezpieczną nagrodę.\n", GetRewardText(safe));
            PrintWrappedLine(textBuffer, LatestTerminalWidth, 0, false);
            break;

        case QQR_Forfeit:
            SetColorRGBPreset(LoadedSettings->SupportColor, false);
            printf("Zrezygnowałeś z gry i wygrałeś %s\n", GetRewardText(i - 1));
            break;
    }

    ResetColor();

    ListDestroy(questions, false);

    WaitForEnter();
}