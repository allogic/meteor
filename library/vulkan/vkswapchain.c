#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <common/macros.h>

#include <platform/nativewindow.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkswapchain.h>

struct xVkSwapChain_t {
	VkSwapchainKHR xSwapChain;
	uint32_t nImageCount;
	VkImage* pxImages;
	VkImageView* pxImageViews;
};

static void VkSwapChain_CreateSwapChain(struct xVkSwapChain_t* pxVkSwapChain, struct xVkInstance_t* pxVkInstance) {
	VkSurfaceCapabilitiesKHR xSurfaceCapabilities;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(VkInstance_GetPhysicalDevice(pxVkInstance), VkInstance_GetSurface(pxVkInstance), &xSurfaceCapabilities));

	uint32_t nMinImageCount = xSurfaceCapabilities.minImageCount + 1;

	VkSwapchainCreateInfoKHR xSwapChaincreateInfo;
	memset(&xSwapChaincreateInfo, 0, sizeof(xSwapChaincreateInfo));
	xSwapChaincreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	xSwapChaincreateInfo.surface = VkInstance_GetSurface(pxVkInstance);
	xSwapChaincreateInfo.minImageCount = nMinImageCount;
	xSwapChaincreateInfo.imageFormat = VkInstance_GetPreferedSurfaceFormat(pxVkInstance);
	xSwapChaincreateInfo.imageColorSpace = VkInstance_GetPreferedSurfaceColorSpace(pxVkInstance);
	xSwapChaincreateInfo.imageExtent.width = NativeWindow_GetWidth();
	xSwapChaincreateInfo.imageExtent.height = NativeWindow_GetHeight();
	xSwapChaincreateInfo.imageArrayLayers = 1;
	xSwapChaincreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	xSwapChaincreateInfo.preTransform = xSurfaceCapabilities.currentTransform;
	xSwapChaincreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	xSwapChaincreateInfo.presentMode = VkInstance_GetPreferedPresentMode(pxVkInstance);
	xSwapChaincreateInfo.clipped = VK_TRUE;
	xSwapChaincreateInfo.oldSwapchain = VK_NULL_HANDLE;

	int32_t nGraphicsQueueIndex = VkInstance_GetGraphicsQueueIndex(pxVkInstance);
	int32_t nPresentQueueIndex = VkInstance_GetPresentQueueIndex(pxVkInstance);

	if (nGraphicsQueueIndex == nPresentQueueIndex) {
		xSwapChaincreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		xSwapChaincreateInfo.queueFamilyIndexCount = 0;
		xSwapChaincreateInfo.pQueueFamilyIndices = 0;
	} else {
		uint32_t anQueueFamilies[] = { nGraphicsQueueIndex, nPresentQueueIndex };

		xSwapChaincreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		xSwapChaincreateInfo.queueFamilyIndexCount = ARRAY_LENGTH(anQueueFamilies);
		xSwapChaincreateInfo.pQueueFamilyIndices = anQueueFamilies;
	}

	VK_CHECK(vkCreateSwapchainKHR(VkInstance_GetDevice(pxVkInstance), &xSwapChaincreateInfo, 0, &pxVkSwapChain->xSwapChain));
}

static void VkSwapChain_CreateImageViews(struct xVkSwapChain_t* pxVkSwapChain, struct xVkInstance_t* pxVkInstance) {
	VK_CHECK(vkGetSwapchainImagesKHR(VkInstance_GetDevice(pxVkInstance), pxVkSwapChain->xSwapChain, &pxVkSwapChain->nImageCount, 0));
	pxVkSwapChain->pxImages = (VkImage*)malloc(sizeof(VkImage) * pxVkSwapChain->nImageCount);
	pxVkSwapChain->pxImageViews = (VkImageView*)malloc(sizeof(VkImageView) * pxVkSwapChain->nImageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(VkInstance_GetDevice(pxVkInstance), pxVkSwapChain->xSwapChain, &pxVkSwapChain->nImageCount, pxVkSwapChain->pxImages));

	VkImageViewCreateInfo xImageViewCreateInfo;

	for (uint32_t i = 0; i < pxVkSwapChain->nImageCount; ++i) {
		memset(&xImageViewCreateInfo, 0, sizeof(xImageViewCreateInfo));
		xImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		xImageViewCreateInfo.image = pxVkSwapChain->pxImages[i];
		xImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		xImageViewCreateInfo.format = VkInstance_GetPreferedSurfaceFormat(pxVkInstance);
		xImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		xImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		xImageViewCreateInfo.subresourceRange.levelCount = 1;
		xImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		xImageViewCreateInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(VkInstance_GetDevice(pxVkInstance), &xImageViewCreateInfo, 0, &pxVkSwapChain->pxImageViews[i]));
	}
}

struct xVkSwapChain_t* VkSwapChain_Alloc(struct xVkInstance_t* pxVkInstance) {
	struct xVkSwapChain_t* pxVkSwapChain = (struct xVkSwapChain_t*)calloc(1, sizeof(struct xVkSwapChain_t));

	VkSwapChain_CreateSwapChain(pxVkSwapChain, pxVkInstance);
	VkSwapChain_CreateImageViews(pxVkSwapChain, pxVkInstance);

	return pxVkSwapChain;
}

void VkSwapChain_Free(struct xVkSwapChain_t* pxVkSwapChain, struct xVkInstance_t* pxVkInstance) {
	for (uint32_t i = 0; i < pxVkSwapChain->nImageCount; ++i) {
		vkDestroyImageView(VkInstance_GetDevice(pxVkInstance), pxVkSwapChain->pxImageViews[i], 0);
	}

	free(pxVkSwapChain->pxImageViews);
	free(pxVkSwapChain->pxImages);

	vkDestroySwapchainKHR(VkInstance_GetDevice(pxVkInstance), pxVkSwapChain->xSwapChain, 0);

	free(pxVkSwapChain);
}

VkSwapchainKHR VkSwapChain_GetSwapChain(struct xVkSwapChain_t* pxVkSwapChain) {
	return pxVkSwapChain->xSwapChain;
}

uint32_t VkSwapChain_GetImageCount(struct xVkSwapChain_t* pxVkSwapChain) {
	return pxVkSwapChain->nImageCount;
}

VkImageView VkSwapChain_GetImageView(struct xVkSwapChain_t* pxVkSwapChain, uint32_t nIndex) {
	return pxVkSwapChain->pxImageViews[nIndex];
}
