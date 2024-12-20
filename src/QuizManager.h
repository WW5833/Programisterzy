#ifndef _INC_QUIZ_MANAGER_H
#define _INC_QUIZ_MANAGER_H 

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

QuestionListHeader* ListCreate();
void ListDestroy(QuestionListHeader* list, bool destoryData);
void ListAdd(QuestionListHeader* list, Question* data);
Question* ListGetAt(QuestionListHeader* list, int index);

int LoadQuestions();
QuestionListHeader* GetQuestionList();
Question* GetRandomQuestion();

#endif // !_INC_QUIZ_MANAGER_H
