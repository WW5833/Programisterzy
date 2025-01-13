#ifndef _INC_IOHELPER_H
#define _INC_IOHELPER_H

#include <stdbool.h>
#include "IOHelper.Utils.h"
#include <stdlib.h>
#include <stdio.h>

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

void SetThisConsoleTitle(const char* title);

void EnableMouseInput(bool enable);

void UpdateTerminalSize();

#define MOUSE_LEFT_BUTTON 1
#define MOUSE_RIGHT_BUTTON 2

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
    void (*clickHandler)(int, int, int, void *),
    void (*doubleClickHandler)(int, int, int, void *),
    void (*scrollHandler)(bool, int, int, void *),
    void (*moveHandler)(int, int, void *),
    void *data);
void UnsetMouseHandler();

void SetResizeHandler(void (*handler)(int, int, void*), void* data);
void UnsetResizeHandler();

#endif // _INC_IOHELPER_H
