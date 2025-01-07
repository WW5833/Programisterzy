#ifndef _INC_QUIZQUESTIONPAGE_H
#define _INC_QUIZQUESTIONPAGE_H

#include "QuizManager.h"
#include "QuizPage.h"

void PageEnter_QuizQuestion(Question* question, int number, QuizQuestionAbilityStatus* abilities, QuizQuestionResult* outCorrect);
void PageEnter_QuizQuestionPreview(Question* question);

#endif // _INC_QUIZQUESTIONPAGE_H
