#include "QuizManager.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define QUESTIONS_FILE "./questions.txt"
#define BUFFER_SIZE 1024

QuestionListHeader* QuestionList;

QuestionListHeader* GetQuestionList() {
    return QuestionList;
}

QuestionListHeader* ListCreate() {
    QuestionListHeader* list = malloc(sizeof(QuestionListHeader));
    if(list == NULL) {
        perror("Failed to allocate memory for list");
        exit(EXIT_FAILURE);
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;

    return list;
}

void ListDestroy(QuestionListHeader* list, bool destoryData) {
    if(list == NULL) return;

    QuestionListItem* current = list->head;
    QuestionListItem* next;

    while(current != NULL) {
        next = current->next;
        if(destoryData)
            DestroyQuestion(current->data);
        free(current);
        current = next;
    }

    free(list);
}

void ListAdd(QuestionListHeader* list, Question* data) {
    QuestionListItem* node = malloc(sizeof(QuestionListItem));
    if(node == NULL) {
        perror("Failed to allocate memory for list node");
        exit(EXIT_FAILURE);
    }

    node->data = data;
    node->next = NULL;

    if(list->head == NULL) {
        list->head = node;
        list->tail = node;
        node->prev = NULL;
    } else {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }

    list->count++;
}

Question* ListGetAt(QuestionListHeader* list, int index) {
    if(index < 0 || index >= list->count) {
        fprintf(stderr, "Index out of range\n");
        return NULL;
    }

    QuestionListItem* current = list->head;
    for(int i = 0; i < index; i++)
        current = current->next;

    return current->data;
}

int LoadQuestions() {
    ListDestroy(QuestionList, true);
    QuestionList = ListCreate();

    char buffer[BUFFER_SIZE];
    FILE* file = fopen(QUESTIONS_FILE, "r");

    if(file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    int lineId = 0;

    while (!feof(file)) { 
        if (buffer != fgets(buffer, BUFFER_SIZE, file))
            break;

        lineId++;

        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n')
            buffer[len - 1] = 0;

        if(len == 1) continue;

        Question* q = DeserializeQuestion(buffer);

        if(q == NULL) {
            fprintf(stderr, "%s:%d | Failed to deserialize question: \"%s\"\n", QUESTIONS_FILE, lineId, buffer);
            exit(EXIT_FAILURE);
        }

        if(q->Id == -1) {
            q->Id = QuestionList->count + 1;
        }

        QuestionListItem* current = QuestionList->head;
        while (current != NULL)
        {
            if(current->data->Id == q->Id) {
                fprintf(stderr, "%s:%d | Question with id %d already exists\n", QUESTIONS_FILE, lineId, q->Id);
                exit(EXIT_FAILURE);
            }

            current = current->next;
        }
        
        ListAdd(QuestionList, q);
    }

    if(fclose(file) != 0) {
        perror("Failed to close file");
        exit(EXIT_FAILURE);
    }

    return QuestionList->count;
}

Question* GetRandomQuestion() {
    if(QuestionList->count == 0) return NULL;

    int index = rand() % QuestionList->count;
    return ListGetAt(QuestionList, index);
}

QuizData* GenerateQuiz(const char username[30]) {
    if(QuestionList == NULL) {
        LoadQuestions();

        if(QuestionList == NULL) {
            fprintf(stderr, "Failed to load questions\n");
            exit(EXIT_FAILURE);
        }
    }

    if(QuestionList->count < 10) {
        fprintf(stderr, "Minimum of 10 questions are requied\n");
        exit(EXIT_FAILURE);
    }

    QuizData* quiz = malloc(sizeof(QuizData));
    if(quiz == NULL) {
        perror("Failed to allocate memory for quiz data");
        exit(EXIT_FAILURE);
    }

    quiz->seed = time(NULL);
    quiz->abulitiesUsed[0] = false;
    quiz->abulitiesUsed[1] = false;
    quiz->abulitiesUsed[2] = false;
    strcpy(quiz->username, username);
    srand(quiz->seed);

    quiz->questions = ListCreate();
    for(int i = 0; i < 10; i++) {
        Question* q = GetRandomQuestion();

        for (int j = 0; j < i; j++)
        {
            if(quiz->questionIds[j] == q->Id) {
                i--;
                q = NULL;
                break;
            }
        }
        
        if(q == NULL) continue;
        ListAdd(quiz->questions, q);
        quiz->questionIds[i] = q->Id;
        quiz->answers[i] = 0;
        quiz->asnwersCorrect[i] = false;
    }

    return quiz;
}

Question* GetCurrentQuestion(QuizData* quiz) {
    if(quiz == NULL) return NULL;
    int i = 0;
    while(quiz->answers[i] != 0) i++;
    if(i >= 10) return NULL;
    return ListGetAt(quiz->questions, i);
}

void DestroyQuiz(QuizData* quiz) {
    if(quiz == NULL) return;

    ListDestroy(quiz->questions, false);
    free(quiz);
}