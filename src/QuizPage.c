#include "QuizPage.h"
#include <stdbool.h>
#include "QuizManager.h"
#include <stdio.h>
#include <stdlib.h>
#include "PageUtils.h"
#include "QuizQuestionPage.h"
#include "AnsiHelper.h"
#include "Settings.h"

extern Settings* LoadedSettings;

void PageEnter_Quiz()
{
    HideCursor();

    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

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
            abilitiesStatus[j] = QQAS_Avaialable;
        }
        
        PageEnter_QuizQuestion(current->data, i + 1, &abilitiesStatus[0], &correct);
        if(correct != QQR_Correct) break;
        current = current->next;
    }

    int safe = -1;
    if(i > 5) safe = 5;
    else if(i > 2) safe = 2;
    else if(i > 0) safe = 0;

    char textBuffer[100];
    switch (correct)
    {
        case QQR_Correct:
            SetColor(LoadedSettings->CorrectAnswerColor);
            printf("Gratulacje! Wygrałeś 1 000 000zł\n");
            break;

        case QQR_Wrong:
            SetColor(LoadedSettings->WrongAnswerColor);
            sprintf(textBuffer, "Niestety, nie udało Ci się wygrać głównej nagrody ale dostajesz %s, ostatnią bezpieczną nagrodę.\n", GetRewardText(safe));
            PrintWrappedLine(textBuffer, terminalWidth, 0, false);
            break;

        case QQR_Forfeit:
            SetColor(LoadedSettings->SupportColor);
            printf("Zrezygnowałeś z gry i wygrałeś %s\n", GetRewardText(i - 1));
            break;
    }

    ResetColor();

    ListDestroy(questions, false);

    WaitForEnter();
}