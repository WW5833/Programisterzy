#ifndef _INC_QUESTIONDETAILSPAGE_H
#define _INC_QUESTIONDETAILSPAGE_H

#include "QuizManager.h"
#include <stdbool.h>

/// @brief Enter the question details page
/// @param question The question to show
/// @param outDeleted The output if the question was deleted
void PageEnter_QuestionDetails(Question* question, bool* outDeleted);

#endif // _INC_QUESTIONDETAILSPAGE_H
