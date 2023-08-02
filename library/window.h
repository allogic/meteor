#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <stdbool.h>

struct xWindow_t;

struct xWindow_t* Window_Alloc(const char* pcWindowTitle, uint32_t nWidth, uint32_t nHeight);
bool Window_ShouldNotClose(struct xWindow_t* pxWindow);
void Window_PollEvents(struct xWindow_t* pxWindow);
void Window_SwapBuffers(struct xWindow_t* pxWindow);
void Window_Free(struct xWindow_t* pxWindow);

#endif
