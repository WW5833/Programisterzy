#ifndef _INC_IOHELPER_H
#define _INC_IOHELPER_H

#include <stdbool.h>
#include "IOHelper.Utils.h"
#include <stdlib.h>
#include <stdio.h>
#include "Errors.h"

#undef getch
#undef kbhit

int getch();
int kbhit();

int WaitForAnyInput();

void IOLoop();

void InitializeIO();

void ExitApp(int exitCode) __attribute__((noreturn));
void ExitAppWithErrorMessage(int exitCode, const char* message) __attribute__((noreturn));
void ExitAppWithErrorFormat(int exitCode, const char* format, ...) __attribute__((noreturn));

void _internal_PreExitApp();
void _internal_PostExitApp(int exitCode);

void SetThisConsoleTitle(const char* title);

void EnableMouseInput(bool enable);

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

void SetMouseHandler(
    void (*clickHandler)(int, void *),
    void (*doubleClickHandler)(int, void *),
    void (*scrollHandler)(bool, void *),
    void (*moveHandler)(void *),
    void *data);
void UnsetMouseHandler();

void SetResizeHandler(void (*handler)(void*), void* data);
void UnsetResizeHandler();

#endif // _INC_IOHELPER_H
