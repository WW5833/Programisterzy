#include "Question.h"
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>

#define TO_ULL(i) ((unsigned long long)(i))
#define TO_INT(i) ((int)(i))

int DeserializeQuestionId(const char* serializedQuestion, Question* question, int* offset) {
    errno = 0;
    char* end;
    question->Id = strtol(serializedQuestion, &end, 10);
    *offset = TO_INT(end - serializedQuestion) + 1;

    if(serializedQuestion == end && serializedQuestion[0] == ';') {
        question->Id = -1;
        printf("Question missing questionId, will generate later\n");
        return 0;
    } else if(serializedQuestion == end) {
        question->Id = -1;
        fprintf(stderr, "[ERR] Invalid questionId in question: \"%s\"\n", serializedQuestion);
        return -1;
    }

    if(errno == ERANGE) {
        // fprintf(stderr, "[ERR] Invalid questionId in question: %s\n", serializedQuestion);
        perror("Invalid questionId while deserializing question");
        return -1;
    }

    return 0;
}

char buffer[255];
int DeserializeString(const char* serializedQuestion, char** content, int* offset, bool allowEmpty) {
    int i = 0;
    while(serializedQuestion[i] != ';' && serializedQuestion[i] != '\0') {
        buffer[i] = serializedQuestion[i];
        i++;
    }
    
    *content = malloc(TO_ULL(i + 1) * sizeof(char));

    if(*content == NULL) {
        perror("Failed to allocate memory for question text content");
        exit(EXIT_FAILURE);
    }

    for (int j = 0; j < i; j++)
        (*content)[j] = buffer[j];
    (*content)[i] = '\0';
    *offset = i + 1;

    if(!allowEmpty && i == 0) {
        fprintf(stderr, "[ERR] Empty string in question content\n");
        return -1;
    }

    return 0;
}

Question* DeserializeQuestion(char* serializedQuestion) {
    if(serializedQuestion == NULL) return NULL;

    Question* question = malloc(sizeof(Question));

    if(question == NULL) {
        perror("Failed to allocate memory for question");
        exit(EXIT_FAILURE);
    }

    int i;

    if(DeserializeQuestionId(serializedQuestion, question, &i) != 0) 
    {
        fprintf(stderr, "Failed to deserialize question id: \"%s\"\n", serializedQuestion);
        return NULL;
    }
    serializedQuestion = &serializedQuestion[i];

    if(DeserializeString(serializedQuestion, &question->Content, &i, false) != 0) 
    {
        fprintf(stderr, "Failed to deserialize question content: \"%s\"\n", serializedQuestion);
        return NULL;
    }   
    serializedQuestion = &serializedQuestion[i];

    for (int j = 0; j < 4; j++)
    {
        if(DeserializeString(serializedQuestion, &question->Answer[j], &i, false) != 0) 
        {
            fprintf(stderr, "Failed to deserialize question answer[%d]: \"%s\"\n", j, serializedQuestion);
            return NULL;
        }
        serializedQuestion = &serializedQuestion[i];
    }

    if(DeserializeString(serializedQuestion, &question->Help, &i, true) != 0) 
    {
        fprintf(stderr, "Failed to deserialize question help: \"%s\"\n", serializedQuestion);
        return NULL;
    }

    return question;
}

int AppendQuestion(FILE* file, Question* question) {
    if(file == NULL) return -1;
    // Id;Question;Ans1;Ans2;Ans3;Ans4;Help;\n
    fprintf(file, "%d;%s;%s;%s;%s;%s;%s;\n",
        question->Id,
        question->Content,
        question->Answer[0],
        question->Answer[1], 
        question->Answer[2], 
        question->Answer[3], 
        question->Help);

    return 0;
}

int DestroyQuestion(Question* question) {
    if(question == NULL) return -1;

    free(question->Content);
    for (int i = 0; i < 4; i++)
        free(question->Answer[i]);
    free(question->Help);
    free(question);

    return 0;
}
