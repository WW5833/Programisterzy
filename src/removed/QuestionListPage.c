#include "QuestionListPage.h"

#include "PageUtils.h"
#include "AnsiHelper.h"
#include "QuizManager.h"

#include "QuestionEditPage.h"

#include <stdio.h>
#include <stdlib.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define MODIFY_QUESTIONS_FILE(mode, mutator) {\
    FILE* questionFile = fopen(QUESTIONS_FILE, mode);\
\
    if (questionFile == NULL) {\
        perror("Failed to open questions file");\
        exit(EXIT_FAILURE);\
    }\
\
    mutator;\
\
    if(fclose(questionFile) == EOF) {\
        fprintf(stderr, "Failed to close file\n");\
        exit(EXIT_FAILURE);\
    }\
\
    LoadQuestions();\
}

#define QUESTION_LIST_SEPARATOR_LIST 2

int DrawQuestionListPage(QuestionListHeader* list, int selected) {
    int optionCount = 2;
    int terminalWidth, terminalHeight;
    GetTerminalSize(&terminalWidth, &terminalHeight);

    ClearScreen();

    printf("Zarządzaj pytaniami\n");
    printf("[ ] Dodaj pytanie\n");
    printf("[ ] Wróć do menu głównego\n");
    printf("    Lista pytań:\n");

    QuestionListItem* current = list->head;
    while (current != NULL)
    {
        printf("[ ] - [%3d] [%d] %.*s\n", current->data->Id, current->data->ContentLength, MIN(current->data->ContentLength, terminalWidth - 12), current->data->Content);
        current = current->next;
        optionCount++;
    
        if(optionCount > 20) break;
    }

    SetCursorPosition(2, 2 + selected + (selected >= QUESTION_LIST_SEPARATOR_LIST ? 1 : 0));
    printf("*");

    return optionCount;
}

void PageEnter_QuestionList()
{
    HideCursor();

    QuestionListHeader* list = GetQuestionList();

    int selected = 0;
    int optionCount = DrawQuestionListPage(list, selected);

    while (true)
    {
        KeyInputType key = HandleInteractions(true);
        if (key == KEY_ARROW_UP || key == KEY_ARROW_DOWN)
        {
            SetCursorPosition(2, 2 + selected + (selected >= QUESTION_LIST_SEPARATOR_LIST ? 1 : 0));
            printf(" ");

            if(key == KEY_ARROW_UP) {
                selected--;
                if (selected < 0) selected = optionCount - 1;
            }
            else {
                selected++;
                if (selected >= optionCount) selected = 0;
            }

            SetCursorPosition(2, 2 + selected + (selected >= QUESTION_LIST_SEPARATOR_LIST ? 1 : 0));
            printf("*");
        }
        else if (key == KEY_ENTER)
        {
            if(selected == 0) {
                // Add question
                Question* q = malloc(sizeof(Question));

                q->Id = GetMaxQuestionId() + 1;
                q->Content = "";
                q->ContentLength = 0;
                for (int i = 0; i < 4; i++) {
                    q->Answer[i] = "";
                    q->AnswerLength[i] = 0;
                }

                q->Help = NULL;
                q->HelpLength = 0;

                bool save = PageEnter_QuestionEdit(q);

                if (save)
                {
                    MODIFY_QUESTIONS_FILE("a", {
                        AppendQuestion(questionFile, q);
                    });
                }

                optionCount = DrawQuestionListPage(list, selected);
            }
            else if(selected == 1) {
                return; // Exit
            }
            else {
                // Edit question
                Question* q = ListGetAt(list, selected - 2);
                bool save = PageEnter_QuestionEdit(q);

                if (save)
                {
                    if(q->Id == INT_MIN) {
                        ListRemove(list, q);
                    }

                    MODIFY_QUESTIONS_FILE("w", {
                        RewriteQuestions(questionFile, list);
                    });
                }

                optionCount = DrawQuestionListPage(list, selected);
            }
        }
    }
}
