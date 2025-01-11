#ifndef _INC_QUIZMANAGER_H
#define _INC_QUIZMANAGER_H

#include "Question.h"
#include <stdbool.h>

#define QUESTIONS_FILE "./questions.txt"

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

Question* ListGetAt(QuestionListHeader* list, int index);
void ListAdd(QuestionListHeader* list, Question* data);
void ListRemove(QuestionListHeader* list, Question* question);
void ListDestroy(QuestionListHeader* list, bool destoryData);

int LoadQuestions();
QuestionListHeader* GetQuestionList();
Question* GetRandomQuestion();

int GetMaxQuestionId();

QuestionListHeader* GenerateQuiz();

#endif // !_INC_QUIZMANAGER_H
