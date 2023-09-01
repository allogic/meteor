#ifndef METEOR_INSTANCE_H
#define METEOR_INSTANCE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;

struct xInstance_t* VkInstance_Alloc(void);
void VkInstance_Free(struct xInstance_t* pxInstance);

VkPhysicalDevice VkInstance_GetPhysicalDevice(struct xInstance_t* pxInstance);
VkDevice VkInstance_GetDevice(struct xInstance_t* pxInstance);
VkSurfaceKHR VkInstance_GetSurface(struct xInstance_t* pxInstance);
VkFormat VkInstance_GetPreferedSurfaceFormat(struct xInstance_t* pxInstance);
VkColorSpaceKHR VkInstance_GetPreferedSurfaceColorSpace(struct xInstance_t* pxInstance);
VkPresentModeKHR VkInstance_GetPreferedPresentMode(struct xInstance_t* pxInstance);
uint32_t VkInstance_GetGraphicsQueueIndex(struct xInstance_t* pxInstance);
uint32_t VkInstance_GetPresentQueueIndex(struct xInstance_t* pxInstance);
VkQueue VkInstance_GetGraphicsQueue(struct xInstance_t* pxInstance);
VkQueue VkInstance_GetPresentQueue(struct xInstance_t* pxInstance);
VkCommandPool VkInstance_GetCommandPool(struct xInstance_t* pxInstance);

void VkInstance_WaitIdle(struct xInstance_t* pxInstance);
int32_t VkInstance_CheckMemoryType(struct xInstance_t* pxInstance, uint32_t nTypeFilter, VkMemoryPropertyFlags xMemoryPropertyFlags);

#endif
