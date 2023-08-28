#ifndef VK_SWAPCHAIN_H
#define VK_SWAPCHAIN_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xVkInstance_t;
struct xVkSwapChain_t;

struct xVkSwapChain_t* VkSwapChain_Alloc(struct xVkInstance_t* pxVkInstance);
void VkSwapChain_Free(struct xVkSwapChain_t* pxVkSwapChain, struct xVkInstance_t* pxVkInstance);

VkSwapchainKHR VkSwapChain_GetSwapChain(struct xVkSwapChain_t* pxVkSwapChain);
uint32_t VkSwapChain_GetImageCount(struct xVkSwapChain_t* pxVkSwapChain);
VkImageView VkSwapChain_GetImageView(struct xVkSwapChain_t* pxVkSwapChain, uint32_t nIndex);

#endif
