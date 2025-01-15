#include "Question.h"
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "IOHelper.h"
#include "QuizManager.h"

static void AppendQuestion(FILE* file, Question* question);
static void SaveQuestions(QuestionListHeader *list);

static bool DeserializeQuestionId(const char* serializedQuestion, Question* question, int* offset) {
    errno = 0;
    char* end;
    question->Id = strtol(serializedQuestion, &end, 10);
    *offset = (int)(end - serializedQuestion) + 1;

    if(serializedQuestion == end) {
        question->Id = -1;
        fprintf(stderr, ERRMSG_QUESTION_INVALID_QUESTION_ID(serializedQuestion));
        return false;
    }

    if(errno == ERANGE) {
        perror(ERRMSG_QUESTION_INVALID_QUESTION_ID_PERRNO);
        return false;
    }

    return true;
}

static bool DeserializeString(const char* serializedQuestion, char** content, int* offset) {
    char buffer[255];
    int i = 0;
    while(serializedQuestion[i] != ';' && serializedQuestion[i] != '\0') {
        buffer[i] = serializedQuestion[i];
        i++;
    }

    *content = malloc((size_t)(i + 1) * sizeof(char));
    mallocCheck(*content);

    for (int j = 0; j < i; j++)
        (*content)[j] = buffer[j];
    (*content)[i] = '\0';
    *offset = i + 1;

    if(i == 0) {
        fprintf(stderr, ERRMSG_QUESTION_CONTENT_EMPTY);
        return false;
    }

    return true;
}

Question* DeserializeQuestion(char* serializedQuestion) {
    if(serializedQuestion == NULL) return NULL;

    Question* question = malloc(sizeof(Question));
    mallocCheck(question);

    question->Id = -1;
    question->Content = NULL;
    for (int i = 0; i < 4; i++)
        question->Answer[i] = NULL;

    int i;

    if(!DeserializeQuestionId(serializedQuestion, question, &i))
    {
        fprintf(stderr,ERRMSG_QUESTION_FAILED_TO_DESERIALIZE_ID(serializedQuestion));
        DestroyQuestion(question);
        return NULL;
    }

    serializedQuestion = &serializedQuestion[i];
    if(!DeserializeString(serializedQuestion, &question->Content, &i))
    {
        fprintf(stderr, ERRMSG_QUESTION_FAILED_TO_DESERIALIZE_CONTENT(serializedQuestion));
        DestroyQuestion(question);
        return NULL;
    }

    for (int j = 0; j < 4; j++)
    {
        serializedQuestion = &serializedQuestion[i];
        if(!DeserializeString(serializedQuestion, &question->Answer[j], &i))
        {
            fprintf(stderr,  ERRMSG_QUESTION_FAILED_TO_DESERIALIZE_ANSWER(j, serializedQuestion));
            DestroyQuestion(question);
            return NULL;
        }
    }

    return question;
}

Question* CloneQuestion(Question *question) {
    Question* clone = malloc(sizeof(Question));
    mallocCheck(clone);

    clone->Id = question->Id;
    clone->Content = malloc(strlen(question->Content) + 1);
    mallocCheck(clone->Content);
    strcpy(clone->Content, question->Content);

    for (int i = 0; i < 4; i++)
    {
        clone->Answer[i] = malloc(strlen(question->Answer[i]) + 1);
        mallocCheck(clone->Answer[i]);
        strcpy(clone->Answer[i], question->Answer[i]);
    }

    return clone;
}

bool ValidateQuestion(Question* question, char** outMessage) {
    if(question->Id <= 0) {
        *outMessage = VLDFAIL_QUESTION_ID_INVALID;
        return false;
    }

    size_t sumLength = 10;
    if(question->Content == NULL || question->Content[0] == '\0') {
        *outMessage = VLDFAIL_QUESTION_CONTENT_EMPTY;
        return false;
    }
    else {
        sumLength += strlen(question->Content);
    }

    for (int i = 0; i < 4; i++)
    {
        if(question->Answer[i] == NULL || question->Answer[i][0] == '\0') {
            *outMessage = VLDFAIL_QUESTION_ANSWER_EMPTY;
            return false;
        }
        else {
            sumLength += strlen(question->Answer[i]);
        }
    }

    if(sumLength >= QUESTION_MAX_FULL_LENGTH) {
        *outMessage = VLDFAIL_QUESTION_TOO_LONG;
        return false;
    }

    QuestionListHeader* list = GetQuestionList();
    if(list == NULL) {
        *outMessage = VLDFAIL_QUESTION_LIST_NULL;
        return false;
    }

    QuestionListItem* current = list->head;
    while(current != NULL) {
        if(current->data->Id == question->Id) {
            *outMessage = VLDFAIL_QUESTION_DUPLICATE_ID;
            return false;
        }

        current = current->next;
    }

    return true;
}

bool AddQuestion(Question* question, char** outMessage) {
    if(!ValidateQuestion(question, outMessage)) return false;

    QuestionListHeader* list = GetQuestionList();

    if(list == NULL) {
        *outMessage = VLDFAIL_QUESTION_LIST_NULL;
        return false;
    }

    ListAdd(list, question);
    SaveQuestions(list);

    return true;
}

bool EditQuestion(Question* question, char** outMessage) {
    QuestionListHeader* list = GetQuestionList();

    if(list == NULL) {
        *outMessage = VLDFAIL_QUESTION_LIST_NULL;
        return false;
    }
    
    if(!ListContains(list, question)) {
        *outMessage = VLDFAIL_QUESTION_NOT_FOUND;
        return false;
    }

    SaveQuestions(list);

    return true;
}

bool DeleteQuestion(Question* question, char** outMessage) {
    QuestionListHeader* list = GetQuestionList();

    if(list == NULL) {
        *outMessage = VLDFAIL_QUESTION_LIST_NULL;
        return false;
    }

    if(!ListRemove(list, question)) {
        *outMessage = VLDFAIL_QUESTION_NOT_FOUND;
        return false;
    }

    SaveQuestions(list);

    return true;
}

static void SaveQuestions(QuestionListHeader *list)
{
    OpenFileChecked(file, QUESTIONS_FILE, "w");
    QuestionListItem* current = list->head;
    while (current != NULL)
    {
        AppendQuestion(file, current->data);
        current = current->next;
    }

    CloseFileChecked(file);
}

static void AppendQuestion(FILE* file, Question* question) {
    // Id;Question;Ans1;Ans2;Ans3;Ans4;\n
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
