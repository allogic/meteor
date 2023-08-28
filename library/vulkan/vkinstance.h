#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H

#include <stdint.h>
#include <stdbool.h>

#include <vulkan/vulkan.h>

struct xVkInstance_t;

struct xVkInstance_t* VkInstance_Alloc(void);
void VkInstance_Free(struct xVkInstance_t* pxVkInstance);

VkPhysicalDevice VkInstance_GetPhysicalDevice(struct xVkInstance_t* pxVkInstance);
VkDevice VkInstance_GetDevice(struct xVkInstance_t* pxVkInstance);
VkSurfaceKHR VkInstance_GetSurface(struct xVkInstance_t* pxVkInstance);
VkFormat VkInstance_GetPreferedSurfaceFormat(struct xVkInstance_t* pxVkInstance);
VkColorSpaceKHR VkInstance_GetPreferedSurfaceColorSpace(struct xVkInstance_t* pxVkInstance);
VkPresentModeKHR VkInstance_GetPreferedPresentMode(struct xVkInstance_t* pxVkInstance);
uint32_t VkInstance_GetGraphicsQueueIndex(struct xVkInstance_t* pxVkInstance);
uint32_t VkInstance_GetPresentQueueIndex(struct xVkInstance_t* pxVkInstance);
VkQueue VkInstance_GetGraphicsQueue(struct xVkInstance_t* pxVkInstance);
VkQueue VkInstance_GetPresentQueue(struct xVkInstance_t* pxVkInstance);
VkCommandPool VkInstance_GetCommandPool(struct xVkInstance_t* pxVkInstance);

#endif
