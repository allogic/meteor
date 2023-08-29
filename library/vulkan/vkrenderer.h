#ifndef VK_RENDERER_H
#define VK_RENDERER_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xVkInstance_t;
struct xVkSwapChain_t;
struct xVkRenderer_t;
struct xVkBuffer_t;

struct xVkRenderer_t* VkRenderer_Alloc(struct xVkInstance_t* pxVkInstance, struct xVkSwapChain_t* pxVkSwapChain);
void VkRenderer_Free(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance, struct xVkSwapChain_t* pxVkSwapChain);

void VkRenderer_UpdateModelViewProjection(struct xVkRenderer_t* pxVkRenderer, void* pData);
void VkRenderer_Draw(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance, struct xVkSwapChain_t* pxVkSwapChain, struct xVkBuffer_t* pxVkVertexBuffer, struct xVkBuffer_t* pxVkIndexBuffer, uint32_t nIndexCount);

#endif
