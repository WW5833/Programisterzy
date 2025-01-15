#ifndef _INC_IOHELPER_H
#define _INC_IOHELPER_H

#include <stdbool.h>
#include "IOHelper.Utils.h"
#include <stdlib.h>
#include <stdio.h>
#include "Errors.h"

/// @brief Get a character from the input buffer (used instead of conio version)
/// @return The character read from the input buffer
int getch();
/// @brief Check if a key is pressed in the input buffer (used instead of conio version)
/// @return 1 if a key is pressed, 0 otherwise
int kbhit();
/// @brief Wait for any input from the user
/// @return The character read from the input buffer or event Id
int WaitForAnyInput();

/// @brief The main IO loop
void IOLoop();
/// @brief Initialize the IO
void InitializeIO();

/// @brief Exit the application with the given exit code
/// @param exitCode The exit code
void ExitApp(int exitCode) __attribute__((noreturn));
/// @brief Exit the application with the given exit code and message
/// @param exitCode The exit code
/// @param message The message to display
void ExitAppWithErrorMessage(int exitCode, const char* message) __attribute__((noreturn));
/// @brief Exit the application with the given exit code and formatted message
/// @param exitCode The exit code
/// @param format The format string
/// @param ... The arguments for the format string
void ExitAppWithErrorFormat(int exitCode, const char* format, ...) __attribute__((noreturn));

/// @brief Prepare to exit the application
void _internal_PreExitApp();
/// @brief Finalize the exit of the application
void _internal_PostExitApp(int exitCode);

/// @brief Set the title of the console window
void SetThisConsoleTitle(const char* title);
/// @brief Enable or disable mouse input
void EnableMouseInput(bool enable);
/// @brief Refresh the terminal size
void UpdateTerminalSize();

#define MOUSE_LEFT_BUTTON 1
#define MOUSE_RIGHT_BUTTON 2

#define SHIFT_TAB '\xB'

#define VK_END 0x23
#define VK_HOME 0x24
#define VK_LEFT 0x25
#define VK_UP 0x26
#define VK_RIGHT 0x27
#define VK_DOWN 0x28
#define VK_INSERT 0x2D
#define VK_DELETE 0x2E

#define VK_SHIFT_MOD 0x40

/// @brief Set the mouse handler
/// @param clickHandler Handler for mouse click events
/// @param doubleClickHandler Handler for mouse double click events
/// @param scrollHandler Handler for mouse scroll events
/// @param moveHandler Handler for mouse move events
/// @param data The data to pass to the handlers
void SetMouseHandler(
    void (*clickHandler)(int, void *),
    void (*doubleClickHandler)(int, void *),
    void (*scrollHandler)(bool, void *),
    void (*moveHandler)(void *),
    void *data);
/// @brief Unset the mouse handler
void UnsetMouseHandler();

/// @brief Set the resize handler
/// @param handler The handler for resize events
/// @param data The data to pass to the handler
void SetResizeHandler(void (*handler)(void*), void* data);
/// @brief Unset the resize handler
void UnsetResizeHandler();

#endif // _INC_IOHELPER_H
