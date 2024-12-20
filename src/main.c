#include <stdlib.h>
#include <stdio.h>
#include "QuizManager.h"


int main() {
    LoadQuestions();
    QuestionListItem* current = GetQuestionList()->head;
    while (current != NULL)
    {
        printf("Question ID: %d\n", current->data->Id);
        printf("Question Content: %s\n", current->data->Content);
        printf("Question Ans1: %s\n", current->data->Answer[0]);
        printf("Question Ans2: %s\n", current->data->Answer[1]);
        printf("Question Ans3: %s\n", current->data->Answer[2]);
        printf("Question Ans4: %s\n", current->data->Answer[3]);
        printf("Question Help: %s\n", current->data->Help);

        current = current->next;
    }

    return 0;
}