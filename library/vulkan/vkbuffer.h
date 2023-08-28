#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xVkInstance_t;
struct xVkBuffer_t;

struct xVkBuffer_t* VkBuffer_Alloc(struct xVkInstance_t* pxVkInstance, uint64_t wSize, VkBufferUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties);
void VkBuffer_Free(struct xVkBuffer_t* pxBuffer, struct xVkInstance_t* pxVkInstance);

VkBuffer VkBuffer_GetBuffer(struct xVkBuffer_t* pxBuffer);
VkDeviceMemory VkBuffer_GetDeviceMemory(struct xVkBuffer_t* pxBuffer);

void VkBuffer_Copy(struct xVkInstance_t* pxVkInstance, struct xVkBuffer_t* pxVkSourceBuffer, struct xVkBuffer_t* pxVkDestinationBuffer, uint64_t wSize);

#endif
