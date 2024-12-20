#include "QuizManager.h"
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

QuestionListHeader* QuestionList;

QuestionListHeader* GetQuestionList() {
    return QuestionList;
}

int LoadQuestions() {
    ListDestroy(QuestionList, true);
    QuestionList = ListCreate();

    char buffer[BUFFER_SIZE];
    FILE* file = fopen("./questions.txt", "r");

    if(file == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    while (!feof(file)) { 
        if (buffer != fgets(buffer, BUFFER_SIZE, file))
            break;

        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n')
            buffer[len - 1] = 0;

        if(len == 1) continue;

        Question* q = DeserializeQuestion(buffer);

        if(q == NULL) {
            fprintf(stderr, "Failed to deserialize question: \"%s\"\n", buffer);
            exit(EXIT_FAILURE);
        }

        ListAdd(QuestionList, q);
    }

    if(fclose(file) != 0) {
        perror("Failed to close file");
        exit(EXIT_FAILURE);
    }

    return QuestionList->count;
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