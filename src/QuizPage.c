#include "QuizPage.h"
#include <stdbool.h>
#include "QuizManager.h"
#include <stdio.h>
#include <stdlib.h>
#include "PageUtils.h"
#include "QuizQuestionPage.h"
#include "AnsiHelper.h"
#include "Settings.h"
#include <conio.h>

extern Settings* LoadedSettings;

void PageEnter_Quiz()
{
    HideCursor();
    ClearScreen();

    QuestionListHeader* questions = GenerateQuiz();
    if(questions == NULL) return;

    bool abilitiesUsed[3] = {false, false, false};
    bool correct = false;
    QuestionListItem* current = questions->head;
    for(int i = 0; i < 10; i++) {
        PageEnter_QuizQuestion(current->data, i + 1, &abilitiesUsed[0], &correct);
        if(!correct) break;
        current = current->next;
    }

    if(correct) {
        SetColor(LoadedSettings->CorrectAnswerColor);
        printf("Wygrałeś! Gratulacje!\n");
    }
    else {
        SetColor(LoadedSettings->WrongAnswerColor);
        printf("Niestety, nie udało Ci się wygrać.\n");
    }

    ResetColor();

    ListDestroy(questions, false);

    WaitForEnter();
}