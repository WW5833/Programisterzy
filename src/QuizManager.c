#include "QuizManager.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "IOHelper.h"
#include "Popup.h"

#define BUFFER_SIZE 1024

static QuestionListHeader* QuestionList;

QuestionListHeader* GetQuestionList()
{
    return QuestionList;
}

QuestionListItem* ListGetAtInternal(QuestionListHeader* list, int index)
{
    if(index < 0 || index >= list->count)
    {
        fprintf(stderr, ERRMSG_INDEX_OUT_OF_RANGE);
        return NULL;
    }

    QuestionListItem* current = list->head;
    for(int i = 0; i < index; i++)
        current = current->next;

    return current;
}

QuestionListHeader* ListCreate()
{
    QuestionListHeader* list = malloc(sizeof(QuestionListHeader));
    mallocCheck(list);

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;

    return list;
}

void ListClear(QuestionListHeader* list, bool destoryData)
{
    if(list == NULL) return;

    QuestionListItem* current = list->head;
    QuestionListItem* next;

    while(current != NULL)
    {
        next = current->next;
        if(destoryData)
            DestroyQuestion(current->data);
        free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

void ListDestroy(QuestionListHeader* list, bool destoryData)
{
    if(list == NULL) return;

    ListClear(list, destoryData);

    free(list);
}

bool ListContains(QuestionListHeader *list, Question *question)
{
    if(list == NULL) return false;

    QuestionListItem* current = list->head;
    while (current != NULL)
    {
        if(current->data == question)
        {
            return true;
        }

        current = current->next;
    }

    return false;
}

void ListAdd(QuestionListHeader* list, Question* data)
{
    QuestionListItem* node = malloc(sizeof(QuestionListItem));
    mallocCheck(node);

    node->data = data;
    node->next = NULL;

    if(list->head == NULL)
    {
        list->head = node;
        list->tail = node;
        node->prev = NULL;
    }
    else
    {
        list->tail->next = node;
        node->prev = list->tail;
        list->tail = node;
    }

    list->count++;
}

void ListInsert(QuestionListHeader* list, int index, Question* data)
{
    QuestionListItem* next = ListGetAtInternal(list, index);

    if(next == NULL)
    {
        ListAdd(list, data);
        return;
    }

    QuestionListItem* node = malloc(sizeof(QuestionListItem));
    mallocCheck(node);

    node->data = data;
    node->next = next;
    node->prev = next->prev;

    if(next->prev == NULL)
    {
        list->head = node;
    }
    else
    {
        next->prev->next = node;
    }

    next->prev = node;

    list->count++;
}

Question* ListGetAt(QuestionListHeader* list, int index)
{
    if(index < 0 || index >= list->count)
    {
        fprintf(stderr, ERRMSG_INDEX_OUT_OF_RANGE);
        return NULL;
    }

    QuestionListItem* current = list->head;
    for(int i = 0; i < index; i++)
        current = current->next;

    return current->data;
}

bool ListRemove(QuestionListHeader* list, Question* question)
{
    if(list == NULL) return false;

    QuestionListItem* current = list->head;
    while (current != NULL)
    {
        if(current->data == question)
        {
            if(current->prev != NULL)
            {
                current->prev->next = current->next;
            }
            else
            {
                list->head = current->next;
            }

            if(current->next != NULL)
            {
                current->next->prev = current->prev;
            }
            else
            {
                list->tail = current->prev;
            }

            list->count--;
            free(current);
            return true;
        }

        current = current->next;
    }

    return false;
}

QuestionListHeader* GetQuestionListCopy()
{
    QuestionListHeader* list = ListCreate();

    QuestionListItem* current = QuestionList->head;
    while (current != NULL)
    {
        ListAdd(list, current->data);
        current = current->next;
    }

    return list;
}

void EnsureQuestionsFileExists()
{
    OpenFileChecked(file, QUESTIONS_FILE, "a");
    CloseFileChecked(file);
}

int LoadQuestionsFromFile()
{
    EnsureQuestionsFileExists();

    if(QuestionList == NULL)
    {
        QuestionList = ListCreate();
    }
    else
    {
        ListClear(QuestionList, true);
    }

    char buffer[BUFFER_SIZE];
    OpenFileChecked(file, QUESTIONS_FILE, "r");

    int lineId = 0;

    while (!feof(file))
    {
        if (buffer != fgets(buffer, BUFFER_SIZE, file))
            break;

        lineId++;

        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n')
            buffer[len - 1] = 0;

        if(len == 1) continue;

        Question* q = DeserializeQuestion(buffer);

        if(q == NULL)
        {
            ExitAppWithErrorFormat(EXIT_FAILURE, ERRMSG_QUESTION_FAILED_TO_DESERIALIZE(QUESTIONS_FILE, lineId, buffer));
        }

        if(q->Id <= 0)
        {
            ExitAppWithErrorFormat(EXIT_FAILURE, ERRMSG_QUESTION_INVALID_OUT_OF_RANGE_QUESTION_ID(QUESTIONS_FILE, lineId, q->Id));
        }

        QuestionListItem* current = QuestionList->head;
        while (current != NULL)
        {
            if(current->data->Id == q->Id)
            {
                ExitAppWithErrorFormat(EXIT_FAILURE, ERRMSG_QUESTION_DUPLICATE_QUESTION_ID(QUESTIONS_FILE, lineId, q->Id));
            }

            current = current->next;
        }

        ListAdd(QuestionList, q);
    }

    CloseFileChecked(file);

    return QuestionList->count;
}

Question* GetRandomQuestion()
{
    if(QuestionList->count == 0) return NULL;

    int index = rand() % QuestionList->count;
    return ListGetAt(QuestionList, index);
}

int GetMaxQuestionId()
{
    int max = 0;
    QuestionListItem* current = QuestionList->head;
    while (current != NULL)
    {
        if(current->data->Id > max)
            max = current->data->Id;
        current = current->next;
    }

    return max;
}

QuestionListHeader* GenerateQuiz()
{
    if(QuestionList == NULL)
    {
        LoadQuestionsFromFile();

        if(QuestionList == NULL)
        {
            ExitAppWithErrorMessage(EXIT_FAILURE, ERRMSG_QUESTION_FAILED_TO_LOAD);
        }
    }

    if(QuestionList->count < 10)
    {
        ShowAlertPopup("Aby rozpocząć quiz, potrzebujesz minimum 10 pytań.", 40);
        return NULL;
    }

    int questionIds[10];
    QuestionListHeader* questions = ListCreate();
    for(int i = 0; i < 10; i++)
    {
        Question* q = GetRandomQuestion();

        for (int j = 0; j < i; j++)
        {
            if(questionIds[j] == q->Id)
            {
                i--;
                q = NULL;
                break;
            }
        }

        if(q == NULL) continue;
        ListAdd(questions, q);
        questionIds[i] = q->Id;
    }

    return questions;
}
