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

    bool abilitiesUsed[3] = {false, false, false};
    QuizQuestionResult correct;
    QuestionListItem* current = questions->head;
    for(int i = 0; i < 10; i++) {
        PageEnter_QuizQuestion(current->data, i + 1, &abilitiesUsed[0], &correct);
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