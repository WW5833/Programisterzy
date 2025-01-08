#ifndef _INC_QUIZPAGE_H
#define _INC_QUIZPAGE_H

typedef enum {
    QQR_Wrong,
    QQR_Correct,
    QQR_Forfeit,
} QuizQuestionResult;

typedef enum {
    // Avaiable to use
    QQAS_Avaialable = 0,
    // Active now (Used in current question)
    QQAS_Active = 1,
    // Unavailable to use
    QQAS_Unavailable = 2,
    // Selected by user (Awaiting confirmation)
    QQAS_Selected = 3,
} QuizQuestionAbilityStatus;

void PageEnter_Quiz();

#endif // _INC_QUIZPAGE_H