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

typedef struct {
    unsigned int seed;
    QuestionListHeader* questions;
    int questionIds[10];
    int currentQuestion;
    char username[30];
    bool abulitiesUsed[3];
} QuizData;

Question* ListGetAt(QuestionListHeader* list, int index);
void ListAdd(QuestionListHeader* list, Question* data);
void ListRemove(QuestionListHeader* list, Question* question);

int LoadQuestions();
QuestionListHeader* GetQuestionList();
Question* GetRandomQuestion();
void RewriteQuestions(FILE* file, QuestionListHeader* list);

int GetMaxQuestionId();

QuizData* GenerateQuiz(const char username[30]);
Question* GetCurrentQuestion(QuizData* quiz);
void DestroyQuiz(QuizData* quiz);

#endif // !_INC_QUIZMANAGER_H
