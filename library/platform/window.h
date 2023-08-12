#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <stdbool.h>

struct xWindow_t;

struct xWindow_t* Window_Alloc(const char* pcWindowTitle, uint32_t nWidth, uint32_t nHeight);
void Window_Free(struct xWindow_t* pxWindow);

bool Window_ShouldNotClose(struct xWindow_t* pxWindow);
void Window_PollEvents(struct xWindow_t* pxWindow);

#ifdef OS_WINDOWS
void* Window_GetWindowHandle(struct xWindow_t* pxWindow);
void* Window_GetModuleHandle(struct xWindow_t* pxWindow);
#endif

#ifdef OS_LINUX
void* Window_GetDisplayHandle(struct xWindow_t* pxWindow);
void* Window_GetSurfaceHandle(struct xWindow_t* pxWindow);
#endif

uint32_t Window_GetWidth(struct xWindow_t* pxWindow);
uint32_t Window_GetHeight(struct xWindow_t* pxWindow);

#endif
