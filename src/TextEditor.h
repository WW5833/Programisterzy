#ifndef _INC_TEXTEDITOR_H
#define _INC_TEXTEDITOR_H

/// @brief TextEditor result
typedef enum {
    /// @brief User has canceled editing
    TextEditorResult_Cancelled,
    /// @brief User has completed editing
    TextEditorResult_Completed,
    /// @brief Editor run out of lines
    TextEditorResult_OutOfLines,
    /// @brief Terminal was resized
    TextEditorResult_WindowResized,
    /// @brief Arrow up was pressed
    TextEditorResult_ArrowUp,
    /// @brief Arrow down was pressed
    TextEditorResult_ArrowDown,
} TextEditorResult;

/// @brief Starts TextEditor
/// @param buffer Buffer to load initial value from and to store the result
/// @param bufferLength Number of items in buffer
/// @param beginX TextEditor top-left character X
/// @param beginY TextEditor top-left character Y
/// @param maxLines Maximum number of lines
/// @param rightFiller TextEditor will clear screen on the right side of itself, this string will be placed at the end of each cleared line
/// @return TextEditor result
TextEditorResult OpenTextEditor(char** buffer, int* bufferLength, int beginX, int beginY, int maxLines, const char* rightFiller);

#endif // _INC_TEXTEDITOR_H
