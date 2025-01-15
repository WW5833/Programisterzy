#ifndef _INC_QUESTIONEDITPAGE_H
#define _INC_QUESTIONEDITPAGE_H

#include "Question.h"
#include "QuizManager.h"
#include <stdbool.h>

/// @brief Enter the question edit page
/// @param question The question to edit
/// @param newQuestion Whether the question is new
/// @return true if the question was edited, false otherwise
bool PageEnter_QuestionEdit(Question* question, bool newQuestion);

#endif // _INC_QUESTIONEDITPAGE_H
