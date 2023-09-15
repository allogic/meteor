#ifndef METEOR_PLATFORM_NATIVEWINDOW_H
#define METEOR_PLATFORM_NATIVEWINDOW_H

#include <stdint.h>
#include <stdbool.h>

void NativeWindow_Alloc(const char* pcTitle, uint32_t nWidth, uint32_t nHeight);
void NativeWindow_Free(void);

bool NativeWindow_ShouldClose(void);
bool NativeWindow_HasResized(void);
void NativeWindow_PollEvents(void);

#ifdef OS_WINDOWS
void* NativeWindow_GetWindowHandle(void);
void* NativeWindow_GetModuleHandle(void);
#endif

#ifdef OS_LINUX
void* NativeWindow_GetDisplayHandle(void);
void* NativeWindow_GetSurfaceHandle(void);
#endif

uint32_t NativeWindow_GetWidth(void);
uint32_t NativeWindow_GetHeight(void);

#endif
