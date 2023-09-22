#ifndef METEOR_VULKAN_INSTANCE_H
#define METEOR_VULKAN_INSTANCE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;

struct xInstance_t* Instance_Alloc(void);
void Instance_Free(struct xInstance_t* pxInstance);

VkPhysicalDevice Instance_GetPhysicalDevice(struct xInstance_t* pxInstance);
VkDevice Instance_GetDevice(struct xInstance_t* pxInstance);
VkSurfaceKHR Instance_GetSurface(struct xInstance_t* pxInstance);
VkFormat Instance_GetPreferedSurfaceFormat(struct xInstance_t* pxInstance);
VkColorSpaceKHR Instance_GetPreferedSurfaceColorSpace(struct xInstance_t* pxInstance);
VkPresentModeKHR Instance_GetPreferedPresentMode(struct xInstance_t* pxInstance);
uint32_t Instance_GetGraphicAndComputeQueueIndex(struct xInstance_t* pxInstance);
uint32_t Instance_GetPresentQueueIndex(struct xInstance_t* pxInstance);
VkQueue Instance_GetGraphicQueue(struct xInstance_t* pxInstance);
VkQueue Instance_GetComputeQueue(struct xInstance_t* pxInstance);
VkQueue Instance_GetPresentQueue(struct xInstance_t* pxInstance);
VkCommandPool Instance_GetCommandPool(struct xInstance_t* pxInstance);

void Instance_GraphicQueueWaitIdle(struct xInstance_t* pxInstance);
void Instance_ComputeQueueWaitIdle(struct xInstance_t* pxInstance);
void Instance_PresentQueueWaitIdle(struct xInstance_t* pxInstance);
void Instance_DeviceWaitIdle(struct xInstance_t* pxInstance);

int32_t Instance_CheckMemoryType(struct xInstance_t* pxInstance, uint32_t nTypeFilter, VkMemoryPropertyFlags xMemoryPropertyFlags);

#endif
