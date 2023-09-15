#ifndef METEOR_VULKAN_SWAPCHAIN_H
#define METEOR_VULKAN_SWAPCHAIN_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xSwapChain_t;

struct xSwapChain_t* SwapChain_Alloc(struct xInstance_t* pxInstance);
void SwapChain_Free(struct xSwapChain_t* pxSwapChain, struct xInstance_t* pxInstance);

VkSwapchainKHR SwapChain_GetSwapChain(struct xSwapChain_t* pxSwapChain);
uint32_t SwapChain_GetImageCount(struct xSwapChain_t* pxSwapChain);
VkRenderPass SwapChain_GetRenderPass(struct xSwapChain_t* pxSwapChain);
VkFramebuffer SwapChain_GetFrameBuffer(struct xSwapChain_t* pxSwapChain, uint32_t nIndex);

#endif
