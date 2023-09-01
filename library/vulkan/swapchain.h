#ifndef METEOR_SWAPCHAIN_H
#define METEOR_SWAPCHAIN_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xSwapChain_t;

struct xSwapChain_t* VkSwapChain_Alloc(struct xInstance_t* pxInstance);
void VkSwapChain_Free(struct xSwapChain_t* pxSwapChain, struct xInstance_t* pxInstance);

VkSwapchainKHR VkSwapChain_GetSwapChain(struct xSwapChain_t* pxSwapChain);
uint32_t VkSwapChain_GetImageCount(struct xSwapChain_t* pxSwapChain);
VkRenderPass VkSwapChain_GetRenderPass(struct xSwapChain_t* pxSwapChain);
VkFramebuffer VkSwapChain_GetFrameBuffer(struct xSwapChain_t* pxSwapChain, uint32_t nIndex);

#endif
