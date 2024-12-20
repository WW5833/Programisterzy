#include "Question.h"
#include <stdlib.h>

int DeserializeQuestionId(const char* serializedQuestion, Question* question) {
    errno = 0;
    char* end;
    question->Id = strtol(serializedQuestion, &end, 10);
    if(serializedQuestion == end) {
        question->Id = -1;
        printf("Question missing questionId, will generate later");
        return 0;
    }

    if(errno == ERANGE) {
        fprintf(stderr, "[ERR] Invalid questionId in question: %s", serializedQuestion);
        return -1;
    }

    return 0;
}

Question DeserializeQuestion(char* serializedQuestion) {
    Question question;
    if(serializedQuestion == NULL) return question;

    int i = 1, j = 0, segment = 0;
    char c = serializedQuestion[0];
    char* end;
    while(c != ';' && c != '\0') {

        switch (segment)
        {
        case 0:
            segment++;
            errno = 0;
            question.Id = strtol(serializedQuestion[j], &end, 10);
            if(c == end) {
                question.Id = -1;
                j = i + (end - serializedQuestion[j]);
                break;
            }

            if(errno == ERANGE) {
                printf("Invalid questionId in question: %s", serializedQuestion);
                return question;
            }
            break;
        
        default:
            break;
        }

        c = serializedQuestion[i];
        i++;
    }

}

int AppendQuestion(FILE* file, Question question) {
    if(file == NULL) return -1;
    // Id;Question;Ans1;Ans2;Ans3;Ans4;Help;\n
    fprintf(file, "%d;%s;%s;%s;%s;%s;%s;\n",
        question.Id,
        question.Content,
        question.Answer[0],
        question.Answer[1], 
        question.Answer[2], 
        question.Answer[3], 
        question.Help);

    return 0;
}