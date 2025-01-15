#ifndef _INC_QUESTION_H
#define _INC_QUESTION_H

#include <stdio.h>
#include <stdbool.h>

/// @brief The question structure
typedef struct
{
    /// @brief The id of the question
    int Id;
    /// @brief The content of the question
    char* Content;
    /// @brief The correct answer index
    char* Answer[4];
} Question;

/// @brief Deserialize the question from the serialized string
/// @param serializedQuestion The serialized question string
/// @return The deserialized question
Question* DeserializeQuestion(char* serializedQuestion);
/// @brief Destroy the question
/// @param question The question to destroy
void DestroyQuestion(Question* question);

/// @brief Clone the question
/// @param question The question to clone
/// @return The cloned question
Question* CloneQuestion(Question* question);
/// @brief Validate the question
/// @param question The question to validate
/// @param outMessage The output message
/// @return true if valid, false otherwise
bool ValidateQuestion(Question* question, char** outMessage);
/// @brief Add the question
/// @param question The question to add
/// @param outMessage The output message
/// @return true if added, false otherwise
bool AddQuestion(Question* question, char** outMessage);
/// @brief Edit the question
/// @param question The question to edit
/// @param outMessage The output message
/// @return true if edited, false otherwise
bool EditQuestion(Question* question, char** outMessage);
/// @brief Delete the question
/// @param question The question to delete
/// @param outMessage The output message
/// @return true if deleted, false otherwise
bool DeleteQuestion(Question* question, char** outMessage);

#endif // !_INC_QUESTION_H
