#include "TextEditor.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "AnsiHelper.h"
#include "IOHelper.h"
#include <ctype.h>
#include "Popup.h"
#include "TextHelper.h"

typedef struct CharacterData
{
    union CharacterDataType
    {
        char ascii;
        char utf8[4];
    } data;

    int size;

    struct CharacterData* next;
    struct CharacterData* prev;
} CharacterData;

static void PrintChar(CharacterData* node) {
    if(node->size == 1) {
        printf("%c", node->data.ascii);
    } else {
        for (int i = 0; i < node->size; i++)
        {
            printf("%c", node->data.utf8[i]);
        }
    }
}

typedef struct WordData
{
    union WordDataType
    {
        char space;
        CharacterData* word;
    } data;

    int length;
    struct WordData* next;
    struct WordData* prev;
} WordData;

static WordData* GenerateWordList(CharacterData* buffer) {
    WordData* head = malloc(sizeof(WordData));
    head->length = 0;
    head->next = NULL;
    head->prev = NULL;

    WordData* current = head;

    CharacterData* node = buffer;

    if(node->data.ascii == '\0') return NULL;

    while(node->size != 0) {
        if(node->size == 1 && node->data.ascii == ' ') {
            WordData* word = malloc(sizeof(WordData));
            word->data.space = ' ';
            word->length = 0;
            word->next = NULL;
            word->prev = current;

            current->next = word;
            current = word;
        }
        else if(current->length == 0) {
            WordData* word = malloc(sizeof(WordData));
            word->data.word = node;
            word->length = 1;
            word->next = NULL;
            word->prev = current;

            current->next = word;
            current = word;
        }
        else {
            current->length++;
        }

        node = node->next;

        if(node == NULL) {
            ExitAppWithErrorFormat(EXIT_FAILURE, "Unexpected end of buffer, null terminator not found");
        }
    }

    current = head->next;
    current->prev = NULL;
    free(head);
    return current;
}

static void DestroyWordList(WordData* head) {
    WordData* current = head;
    while(current != NULL) {
        WordData* next = current->next;
        free(current);
        current = next;
    }
}

static int PrintWord(WordData* word) {
    if(word->length == 0) {
        printf(" ");
        return 1;
    }

    CharacterData* node = word->data.word;
    for (int i = 0; i < word->length; i++)
    {
        PrintChar(node);
        node = node->next;
    }

    return word->length;
}

typedef struct
{
    const char* rightFiller;
    int rightFillerLength;

    int beginX, beginY;
    int width, maxLines;

    CharacterData* buffer;
    int bufferLength;
    int cursorPosition;

    bool resized;
} TextEditorData;

typedef enum {
    RTR_None,
    RTR_Cancelled,
    RTR_Completed,
    RTR_Update,
    RTR_ArrowUp,
    RTR_ArrowDown
} ReadTextResult;

static void OnResize(int width, int height, void* data);
static bool MainLoop(TextEditorData* data, TextEditorResult* result);
static bool DrawBufferContent(TextEditorData* data, TextEditorResult* result);
static int BufferToString(char** dest, CharacterData* src);
static int StringToBuffer(TextEditorData* data, char* src, int srcLength);
static ReadTextResult ReadText(TextEditorData* data);
static CharacterData* GetAt(TextEditorData* data, int index);
static bool RemoveAt(TextEditorData* data, int index);
static void InsertAt(TextEditorData* data, int index, CharacterData* node);
static void RemoveAll(TextEditorData* data);
static bool InsertCharacter(TextEditorData* data, int index, char c);

extern int LatestTerminalWidth;

void LoadDefaultBuffer(TextEditorData* data) {
    data->bufferLength = 0;
    data->cursorPosition = 0;

    data->buffer = malloc(sizeof(CharacterData));
    data->buffer->data.ascii = '\0';
    data->buffer->size = 0;

    data->buffer->next = NULL;
    data->buffer->prev = NULL;
}

TextEditorResult OpenTextEditor(char** buffer, int* bufferLength, int beginX, int beginY, int maxLines, const char* rightFiller)
{
    TextEditorData data;
    data.resized = false;

    data.rightFiller = rightFiller;
    data.rightFillerLength = GetStringCharCount(rightFiller);

    data.beginX = beginX;
    data.beginY = beginY;
    data.width = LatestTerminalWidth - (beginX - 1) - data.rightFillerLength;
    data.maxLines = maxLines;

    LoadDefaultBuffer(&data);

    if(buffer != NULL) {
        StringToBuffer(&data, *buffer, *bufferLength);
    }

    TextEditorResult tor;
    if(!DrawBufferContent(&data, &tor)) {
        RemoveAll(&data);
        return tor;
    }

    SetResizeHandler(OnResize, &data);

    while(MainLoop(&data, &tor)) {
        if(data.resized) {
            tor = TextEditorResult_WindowResized;
            break;
        }
    }

    *bufferLength = BufferToString(buffer, data.buffer);
    RemoveAll(&data);

    UnsetResizeHandler();

    return tor;
}

static void OnResize(int width, int height, void* data) {
    TextEditorData* ted = (TextEditorData*)data;
    ted->resized = true;
}

static bool MainLoop(TextEditorData* data, TextEditorResult* result) {
    ReadTextResult rtr = ReadText(data);
    switch(rtr) {
        case RTR_None:
            return true;

        case RTR_Cancelled:
            *result = TextEditorResult_Cancelled;
            return false;

        case RTR_Completed:
            *result = TextEditorResult_Completed;
            return false;

        case RTR_Update:
            return DrawBufferContent(data, result);

        case RTR_ArrowUp:
            *result = TextEditorResult_ArrowUp;
            return false;

        case RTR_ArrowDown:
            *result = TextEditorResult_ArrowDown;
            return false;

        default:
            return false;
    }
}

static void PrintFiller(TextEditorData* data) {
    if(data->rightFiller != NULL) {
        printf("\r" CSR_MOVE_RIGHT(data->beginX + data->width - 1));
        printf("%s", data->rightFiller);
    }
}

static bool DrawBufferContent(TextEditorData* data, TextEditorResult* result) {
    HideCursor();
    SetCursorPosition(data->beginX, data->beginY);
    WordData* wordList = GenerateWordList(data->buffer);

    int currentWidth = 0;

    int cursorX = 0;
    int cursorY = 0;
    int index = 0;
    int lineCount = 1;
    WordData* current = wordList;
    while(current != NULL) {
        if(currentWidth + current->length > data->width) {
            printf(CLR_LINE_END);
            PrintFiller(data);
            printf("\n" CSR_MOVE_RIGHT(data->beginX - 1));
            lineCount++;

            if(lineCount > data->maxLines) {
                *result = TextEditorResult_OutOfLines;
                DestroyWordList(wordList);
                return false;
            }

            currentWidth = 0;
            if(current->length == 0) {
                index++;
                current = current->next; // Skip leading space
                continue;
            }
        }

        int wordLength = PrintWord(current);
        currentWidth += wordLength;

        if(index <= data->cursorPosition && index + wordLength >= data->cursorPosition) {
            cursorX = currentWidth - (index + wordLength - data->cursorPosition);
            cursorY = lineCount - 1;
        }

        current = current->next;
        index += wordLength;
    }

    DestroyWordList(wordList);

    SetCursorPosition(data->beginX + currentWidth, data->beginY + lineCount - 1);
    printf(CLR_LINE_END);
    PrintFiller(data);
    if(data->maxLines > lineCount) {
        SetCursorPosition(data->beginX, data->beginY + lineCount);
        printf(CLR_LINE_END);
        PrintFiller(data);
    }

    SetCursorPosition(data->beginX + cursorX, data->beginY + cursorY);
    ShowCursor();

    return true;
}

static int StringToBuffer(TextEditorData* data, char* src, int srcLength) {
    for (int i = 0; i < srcLength; i++)
    {
        if(src[i] == '\0') break;
        InsertCharacter(data, data->bufferLength, src[i]);
    }

    return data->cursorPosition = data->bufferLength;
}

static int BufferToString(char** dest, CharacterData* src) {
    int length = 0;
    CharacterData* node = src;
    while(node->size != 0) {
        length += node->size;
        node = node->next;
    }

    if(*dest != NULL) {
        free(*dest);
        *dest = NULL;
    }
    *dest = malloc((size_t)(length + 1));

    node = src;
    int index = 0;
    while (node->size != 0)
    {
        if(node->size == 1) {
            (*dest)[index] = node->data.ascii;
            index++;
        } else {
            for (int i = 0; i < node->size; i++)
            {
                (*dest)[index] = node->data.utf8[i];
                index++;
            }
        }

        node = node->next;
    }


    (*dest)[length] = '\0';
    return length;
}


CharacterData* GetAt(TextEditorData* data, int index) {
    CharacterData* node = data->buffer;
    for (int i = 0; i < index; i++) {
        if(node->next == NULL) return NULL;
        node = node->next;
    }

    return node;
}

bool RemoveAt(TextEditorData* data, int index) {
    CharacterData* node = GetAt(data, index);

    if(node == NULL) return false;

    if(node->prev != NULL) {
        node->prev->next = node->next;
    }

    if(node->next != NULL) {
        node->next->prev = node->prev;
    }

    if(node == data->buffer) {
        data->buffer = node->next;
    }

    data->bufferLength--;

    free(node);
    return true;
}

void InsertAt(TextEditorData* data, int index, CharacterData* node) {
    CharacterData* current = GetAt(data, index);
    if(current == NULL) return;

    if(current->prev != NULL) {
        current->prev->next = node;
    }

    node->prev = current->prev;
    node->next = current;
    current->prev = node;

    if(current == data->buffer) {
        data->buffer = node;
    }

    data->bufferLength++;
}

void RemoveAll(TextEditorData* data) {
    CharacterData* node = data->buffer;
    while(node != NULL) {
        CharacterData* next = node->next;
        free(node);
        node = next;
    }

    data->buffer = NULL;
    data->bufferLength = 0;
}

static ReadTextResult ReadText(TextEditorData* data) {
    while(!kbhit()) {
        if(data->resized) return RTR_None;
    }

    int c = getch();

    switch(c) {
        case '\n':
        case '\r':
            return RTR_Completed;

        case ESC:
            return RTR_Cancelled;

        case ';': // Disallow semicolon
            return RTR_None;

        case '\b': {
            if(data->cursorPosition == 0) return RTR_None;

            data->cursorPosition--;
            RemoveAt(data, data->cursorPosition);
            return RTR_Update;
        }

        case '\t':
            return RTR_ArrowDown;
        case SHIFT_TAB:
            return RTR_ArrowUp;

        case ESCAPE_CHAR: {
            switch (getch())
            {
                case VK_LEFT: // Left arrow
                    if(data->cursorPosition == 0) return RTR_None;
                    data->cursorPosition--;
                    return RTR_Update;

                case VK_RIGHT: // Right arrow
                    if(data->cursorPosition == data->bufferLength) return RTR_None;
                    data->cursorPosition++;
                    return RTR_Update;

                case VK_DELETE: // Delete
                    if(data->cursorPosition == data->bufferLength) return RTR_None;
                    RemoveAt(data, data->cursorPosition);
                    return RTR_Update;

                case VK_UP: // Up arrow
                    return RTR_ArrowUp;

                case VK_DOWN: // Down arrow
                    return RTR_ArrowDown;

                case VK_HOME: // Home
                    data->cursorPosition = 0;
                    return RTR_Update;

                case VK_END: // End
                    data->cursorPosition = data->bufferLength;
                    return RTR_Update;

                case VK_DELETE | VK_SHIFT_MOD: // Shift + Delete
                    RemoveAll(data);
                    LoadDefaultBuffer(data);
                    return RTR_Update;

                default:
                    return RTR_None;
            }
        }

        default: {
            if(!(isalnum(c) || ispunct(c) || isspace(c) || (c & 0x80) /* UTF-8 */)) {
                return RTR_None;
            }

            if(InsertCharacter(data, data->cursorPosition, (char)c)) {
                data->cursorPosition++;
            }

            return RTR_Update;
        }
    }
}

static bool InsertCharacter(TextEditorData* data, int index, char c) {
    if(c & 0x80) {
        if((c & 0xC0) == 0xC0) {
            CharacterData* node = malloc(sizeof(CharacterData));
            node->data.utf8[0] = c;
            node->size = 1;

            InsertAt(data, index, node);
            return true;
        }
        else
        {
            CharacterData* node = GetAt(data, index - 1);
            node->data.utf8[node->size] = c;
            node->size++;
            return false;
        }
    }

    CharacterData* node = malloc(sizeof(CharacterData));
    node->data.ascii = (char)c;
    node->size = 1;

    InsertAt(data, index, node);
    return true;
}
