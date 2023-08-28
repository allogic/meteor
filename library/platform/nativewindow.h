#ifndef NATIVE_WINDOW_H
#define NATIVE_WINDOW_H

#include <stdint.h>
#include <stdbool.h>

struct xNativeWindow_t;

struct xNativeWindow_t* NativeWindow_Alloc(const char* pcTitle, uint32_t nWidth, uint32_t nHeight);
void NativeWindow_Free(struct xNativeWindow_t* pxNativeWindow);

bool NativeWindow_ShouldNotClose(struct xNativeWindow_t* pxNativeWindow);
void NativeWindow_PollEvents(struct xNativeWindow_t* pxNativeWindow);

#ifdef OS_WINDOWS
void* NativeWindow_GetWindowHandle(struct xNativeWindow_t* pxNativeWindow);
void* NativeWindow_GetModuleHandle(struct xNativeWindow_t* pxNativeWindow);
#endif

#ifdef OS_LINUX
void* NativeWindow_GetDisplayHandle(struct xNativeWindow_t* pxNativeWindow);
void* NativeWindow_GetSurfaceHandle(struct xNativeWindow_t* pxNativeWindow);
#endif

uint32_t NativeWindow_GetWidth(struct xNativeWindow_t* pxNativeWindow);
uint32_t NativeWindow_GetHeight(struct xNativeWindow_t* pxNativeWindow);

#endif
