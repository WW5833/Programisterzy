#ifndef _INC_QUIZMANAGER_H
#define _INC_QUIZMANAGER_H 

#include "Question.h"
#include <stdbool.h>

typedef struct QuestionListItem
{
    struct QuestionListItem* next;
    struct QuestionListItem* prev;
    Question* data;
} QuestionListItem;

typedef struct QuestionListHeader
{
    QuestionListItem* head;
    QuestionListItem* tail;
    int count;
} QuestionListHeader;

typedef struct {
    unsigned int seed;
    QuestionListHeader* questions;
    int questionIds[10];
    char answers[10];
    char username[30];
    bool asnwersCorrect[10];
    bool abulitiesUsed[3];
} QuizData;

Question* ListGetAt(QuestionListHeader* list, int index);

int LoadQuestions();
QuestionListHeader* GetQuestionList();
Question* GetRandomQuestion();

QuizData* GenerateQuiz(const char username[30]);
Question* GetCurrentQuestion(QuizData* quiz);
void DestroyQuiz(QuizData* quiz);

#endif // !_INC_QUIZ_MANAGER_H
