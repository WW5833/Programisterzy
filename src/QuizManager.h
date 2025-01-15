#ifndef _INC_QUIZMANAGER_H
#define _INC_QUIZMANAGER_H

#include "Question.h"
#include <stdbool.h>

/// @brief The questions file
#define QUESTIONS_FILE "./questions.txt"

/// @brief The questions max line length
#define QUESTION_MAX_FULL_LENGTH 1000

/// @brief Question list item structure
typedef struct QuestionListItem
{
    /// @brief The next item
    struct QuestionListItem* next;
    /// @brief The previous item
    struct QuestionListItem* prev;
    /// @brief The data
    Question* data;
} QuestionListItem;
/// @brief Question list header structure
typedef struct QuestionListHeader
{
    /// @brief The head of the list
    QuestionListItem* head;
    /// @brief The tail of the list
    QuestionListItem* tail;
    /// @brief The count of items in the list
    int count;
} QuestionListHeader;

/// @brief Get item from list on index
/// @param list List to get item from
/// @param index Index of item
/// @return The item
Question* ListGetAt(QuestionListHeader* list, int index);
/// @brief Add item to the list
/// @param list List to add item to
/// @param data Item to add
void ListAdd(QuestionListHeader* list, Question* data);
/// @brief Insert item to the list
/// @param list List to insert item to
/// @param index Index to insert item at
/// @param data Item to insert
void ListInsert(QuestionListHeader* list, int index, Question* data);
/// @brief Remove item from the list
/// @param list List to remove item from
/// @param question Item to remove
/// @return true if removed, false otherwise
bool ListRemove(QuestionListHeader* list, Question* question);
/// @brief Destory the list
/// @param list List to delete
/// @param destoryData Whether to delete data
void ListDestroy(QuestionListHeader* list, bool destoryData);
/// @brief Check if list contains item
/// @param list List to check
/// @param question Item to check
/// @return true if contains, false otherwise
bool ListContains(QuestionListHeader* list, Question* question);

/// @brief Load questions from file
/// @return number of loaded questions
int LoadQuestionsFromFile();
/// @brief Gets pointer to loaded list of questions
/// @return pointer to loaded list of questions
QuestionListHeader* GetQuestionList();
/// @brief Gets pointer to copy of loaded list of questions
/// @return pointer to copy of loaded list of questions
QuestionListHeader* GetQuestionListCopy();
/// @brief Gets a random question
/// @return random question
Question* GetRandomQuestion();

/// @brief Gets the biggest question id
/// @return max question id
int GetMaxQuestionId();

/// @brief Generates list of 10 random questions
/// @return list of 10 random questions for quiz
QuestionListHeader* GenerateQuiz();

#endif // !_INC_QUIZMANAGER_H
