#ifndef _INC_TEXTEDITOR_H
#define _INC_TEXTEDITOR_H

typedef enum {
    TextEditorResult_Cancelled,
    TextEditorResult_Completed,
    TextEditorResult_OutOfLines,
    TextEditorResult_WindowResized,
    TextEditorResult_ArrowUp,
    TextEditorResult_ArrowDown,
} TextEditorResult;

TextEditorResult OpenTextEditor(char** buffer, int* bufferLength, int beginX, int beginY, int maxLines, const char* rightFiller);

#endif // _INC_TEXTEDITOR_H