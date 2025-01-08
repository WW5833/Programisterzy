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
    ClearScreen();

    QuestionListHeader* questions = GenerateQuiz();
    if(questions == NULL) return;

    QuizQuestionAbilityStatus abilitiesStatus[3] = {QQAS_Avaialable, QQAS_Avaialable, QQAS_Avaialable};
    QuizQuestionResult correct;
    QuestionListItem* current = questions->head;
    for(int i = 0; i < 10; i++) {

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

    switch (correct)
    {
        case QQR_Correct:
            SetColor(LoadedSettings->CorrectAnswerColor);
            printf("Wygrałeś! Gratulacje!\n");
            break;

        case QQR_Wrong:
            SetColor(LoadedSettings->WrongAnswerColor);
            printf("Niestety, nie udało Ci się wygrać.\n");
            break;

        case QQR_Forfeit:
            SetColor(LoadedSettings->SupportColor);
            printf("Zrezygnowałeś z gry.\n");
            break;
    }

    ResetColor();

    ListDestroy(questions, false);

    WaitForEnter();
}