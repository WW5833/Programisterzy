#ifndef _INC_QUESTION_H
#define _INC_QUESTION_H

#include <stdio.h>
#include <stdbool.h>

typedef struct
{
    int Id;
    char* Content;
    char* Answer[4];
} Question;

Question* DeserializeQuestion(char* serializedQuestion);
void DestroyQuestion(Question* question);

bool ValidateQuestion(Question* question, char** outMessage);
bool AddQuestion(Question* question, char** outMessage);
bool EditQuestion(Question* question, char** outMessage);
bool DeleteQuestion(Question* question, char** outMessage);

#endif // !_INC_QUESTION_H
