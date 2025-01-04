#ifndef _INC_QUIZQUESTIONPAGE_H
#define _INC_QUIZQUESTIONPAGE_H

#include "QuizManager.h"

void PageEnter_QuizQuestion(Question* question, int number, bool* abilities, bool* outCorrect);
void PageEnter_QuizQuestionPreview(Question* question);

#endif // _INC_QUIZQUESTIONPAGE_H
