#ifndef _INC_ERRORS_H
#define _INC_ERRORS_H

#include <string.h>
#include "DebugCheck.h"
#include "ErrorMessages.h"

#ifdef PROGRAMISTERZY_DEBUG
    #define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
    #define ERROR_ORIGIN_FORMAT ERRMSG_PREFIX "Wystąpił błąd w %s:%d", __FILENAME__, __LINE__
#else
    #define ERROR_ORIGIN_FORMAT ERRMSG_PREFIX "Wystąpił błąd"
#endif

/// @brief Opens a file and checks if it was opened correctly
/// @param file Name of the variable to store file to (it must not exist, it will be created)
/// @param path Path to the file to open
/// @param mode Mode in whitch file will be opened
#define OpenFileChecked(file, path, mode) \
FILE* file = fopen(path, mode); \
if (file == NULL) { \
    perror(ERRMSG_FILE_OPEN_FAILED(path)); \
    ExitAppWithErrorFormat(EXIT_FAILURE, ERROR_ORIGIN_FORMAT); \
}

/// @brief Closes a file and checks if it was closed correctly
/// @param file File to close
#define CloseFileChecked(file) \
if (fclose(file) == EOF) { \
    perror(ERRMSG_FILE_CLOSE_FAILED); \
    ExitAppWithErrorFormat(EXIT_FAILURE, ERROR_ORIGIN_FORMAT); \
}

/// @brief Wrapper for perror, prepares to exit application
#define perror(args) _internal_PreExitApp(); perror(args)

/// @brief Checks if malloc successfully allocated memory
#define mallocCheck(ptr) \
if(ptr == NULL) { \
    perror(ERRMSG_MALLOC_FAILED); \
    ExitAppWithErrorFormat(EXIT_FAILURE, ERROR_ORIGIN_FORMAT); \
}

#endif // _INC_ERRORS_H