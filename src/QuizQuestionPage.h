#ifndef _INC_QUIZQUESTIONPAGE_H
#define _INC_QUIZQUESTIONPAGE_H

#include "QuizManager.h"
#include "QuizPage.h"

/// @brief Enter thie quiz question page
/// @param question Current question
/// @param number Number of question
/// @param abilities Abilities array
/// @param outCorrect Quiz question result
void PageEnter_QuizQuestion(Question* question, int number, QuizQuestionAbilityStatus* abilities, QuizQuestionResult* outCorrect);
/// @brief Enter thie quiz question page in preview mode
/// @param question Quuestion to preview
/// @param showCorrectAnswer If true correct answers will be marked
void PageEnter_QuizQuestionPreview(Question* question, bool showCorrectAnswer);

/// @brief Get reward text
/// @param rewardId Reward id
/// @return Reward text
const char* GetRewardText(int rewardId);

#endif // _INC_QUIZQUESTIONPAGE_H
