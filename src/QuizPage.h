#ifndef _INC_QUIZPAGE_H
#define _INC_QUIZPAGE_H

/// @brief Quiz question result enum
typedef enum
{
    /// @brief Player has answerd wrong
    QQR_Wrong,
    /// @brief Player has answerd correctly
    QQR_Correct,
    /// @brief Player has left the quiz
    QQR_Forfeit,
} QuizQuestionResult;

typedef enum
{
    /// @brief Ability was not used is available
    QQAS_Avaialable = 0,
    /// @brief Ability was used on this question and is available for the duration of it
    QQAS_Active = 1,
    /// @brief Ability is not available, was used before
    QQAS_Unavailable = 2,
    /// @brief Ability is awating use confirmation from user
    QQAS_Selected = 3,
} QuizQuestionAbilityStatus;

/// @brief Enter yhe quiz page
void PageEnter_Quiz();

#endif // _INC_QUIZPAGE_H
