#ifndef _INC_QUESTION_H
#define _INC_QUESTION_H 

#include <stdio.h>

typedef struct
{
    int Id;

    char* Content;
    int ContentLength;

    char* Answer[4];
    int AnswerLength[4];

    char* Help;
    int HelpLength;
} Question;

Question* DeserializeQuestion(char* serializedQuestion);
void AppendQuestion(FILE* file, Question* question);
void DestroyQuestion(Question* question);

#endif // !_INC_QUESTION_H
