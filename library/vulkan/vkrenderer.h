#ifndef VK_RENDERER_H
#define VK_RENDERER_H

#include <vulkan/vulkan.h>

struct xVkInstance_t;
struct xVkSwapChain_t;
struct xVkRenderer_t;

struct xVkRenderer_t* VkRenderer_Alloc(struct xVkInstance_t* pxVkInstance, struct xVkSwapChain_t* pxVkSwapChain);
void VkRenderer_Free(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance, struct xVkSwapChain_t* pxVkSwapChain);

void VkRenderer_Draw(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance, struct xVkSwapChain_t* pxVkSwapChain);

#endif
