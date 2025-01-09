#include "Question.h"
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "PageUtils.h"
#include "IOHelper.h"
#include "TextHelper.h"

bool DeserializeQuestionId(const char* serializedQuestion, Question* question, int* offset) {
    errno = 0;
    char* end;
    question->Id = strtol(serializedQuestion, &end, 10);
    *offset = (int)(end - serializedQuestion) + 1;

    if(serializedQuestion == end) {
        question->Id = -1;
        fprintf(stderr, "[ERR] Invalid questionId in question: \"%s\"\n", serializedQuestion);
        return false;
    }

    if(errno == ERANGE) {
        perror("Invalid questionId while deserializing question");
        return false;
    }

    return true;
}

char buffer[255];
bool DeserializeString(const char* serializedQuestion, char** content, int* offset) {
    int i = 0;
    while(serializedQuestion[i] != ';' && serializedQuestion[i] != '\0') {
        buffer[i] = serializedQuestion[i];
        i++;
    }
    
    *content = malloc((size_t)(i + 1) * sizeof(char));

    if(*content == NULL) {
        perror("Failed to allocate memory for question text content");
        ExitApp(EXIT_FAILURE);
    }

    for (int j = 0; j < i; j++)
        (*content)[j] = buffer[j];
    (*content)[i] = '\0';
    *offset = i + 1;

    if(i == 0) {
        fprintf(stderr, "[ERR] Empty string in question content\n");
        return false;
    }

    return true;
}

Question* DeserializeQuestion(char* serializedQuestion) {
    if(serializedQuestion == NULL) return NULL;

    Question* question = malloc(sizeof(Question));

    if(question == NULL) {
        perror("Failed to allocate memory for question");
        ExitApp(EXIT_FAILURE);
    }

    question->Id = -1;
    question->Content = NULL;
    for (int i = 0; i < 4; i++)
        question->Answer[i] = NULL;

    int i;

    if(!DeserializeQuestionId(serializedQuestion, question, &i)) 
    {
        fprintf(stderr, "Failed to deserialize question id: \"%s\"\n", serializedQuestion);
        DestroyQuestion(question);
        return NULL;
    }
    serializedQuestion = &serializedQuestion[i];

    if(!DeserializeString(serializedQuestion, &question->Content, &i)) 
    {
        fprintf(stderr, "Failed to deserialize question content: \"%s\"\n", serializedQuestion);
        DestroyQuestion(question);
        return NULL;
    }   
    question->ContentLength = GetStringCharCount(question->Content);

    serializedQuestion = &serializedQuestion[i];

    for (int j = 0; j < 4; j++)
    {
        if(!DeserializeString(serializedQuestion, &question->Answer[j], &i)) 
        {
            fprintf(stderr, "Failed to deserialize question answer[%d]: \"%s\"\n", j, serializedQuestion);
            DestroyQuestion(question);
            return NULL;
        }
        serializedQuestion = &serializedQuestion[i];
        question->AnswerLength[j] = GetStringCharCount(question->Answer[j]);
    }

    return question;
}

void AppendQuestion(FILE* file, Question* question) {
    // Id;Question;Ans1;Ans2;Ans3;Ans4;Help;\n
    fprintf(file, "%d;%s;%s;%s;%s;%s;\n",
        question->Id,
        question->Content,
        question->Answer[0],
        question->Answer[1], 
        question->Answer[2], 
        question->Answer[3]);
}

void DestroyQuestion(Question* question) {
    if(question == NULL) return;

    free(question->Content);
    for (int i = 0; i < 4; i++)
        free(question->Answer[i]);
    free(question);
}
