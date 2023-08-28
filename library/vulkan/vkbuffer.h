#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xVkInstance_t;
struct xVkVertex_t;
struct xVkBuffer_t;

struct xVkBuffer_t* VkVertexBuffer_Alloc(struct xVkInstance_t* pxVkInstance, struct xVkVertex_t* pxVertexBuffer, uint32_t nVertexCount);
void VkVertexBuffer_Free(struct xVkBuffer_t* pxBuffer, struct xVkInstance_t* pxVkInstance);

VkBuffer VkBuffer_GetBuffer(struct xVkBuffer_t* pxBuffer);
VkDeviceMemory VkBuffer_GetDeviceMemory(struct xVkBuffer_t* pxBuffer);

#endif
