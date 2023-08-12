#ifndef VULKAN_H
#define VULKAN_H

#include <stdbool.h>

struct xWindow_t;
struct xVulkan_t;

struct xVulkan_t* Vulkan_Alloc(struct xWindow_t* pxWindow);
void Vulkan_Free(struct xVulkan_t* pxVulkan);

bool Vulkan_Draw(struct xVulkan_t* pxVulkan);

#endif
