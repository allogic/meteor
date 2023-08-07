#ifndef VULKAN_H
#define VULKAN_H

#include <stdbool.h>

struct xWindow_t;

bool Vulkan_Alloc(struct xWindow_t* pxWindow);
void Vulkan_Free(void);

#endif
