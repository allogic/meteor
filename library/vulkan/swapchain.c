#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <common/macros.h>

#include <platform/nativewindow.h>

#include <vulkan/instance.h>
#include <vulkan/swapchain.h>

struct xSwapChain_t {
	VkSwapchainKHR xSwapChain;
	uint32_t nImageCount;
	VkImage* pxImages;
	VkImageView* pxImageViews;
	VkRenderPass xRenderPass;
	VkFramebuffer* pxFrameBuffer;
};

static void SwapChain_CreateSwapChain(struct xSwapChain_t* pxSwapChain, struct xInstance_t* pxInstance) {
	VkSurfaceCapabilitiesKHR xSurfaceCapabilities;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Instance_GetPhysicalDevice(pxInstance), Instance_GetSurface(pxInstance), &xSurfaceCapabilities));

	uint32_t nMinImageCount = xSurfaceCapabilities.minImageCount + 1;

	VkSwapchainCreateInfoKHR xSwapChaincreateInfo;
	memset(&xSwapChaincreateInfo, 0, sizeof(xSwapChaincreateInfo));
	xSwapChaincreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	xSwapChaincreateInfo.surface = Instance_GetSurface(pxInstance);
	xSwapChaincreateInfo.minImageCount = nMinImageCount;
	xSwapChaincreateInfo.imageFormat = Instance_GetPreferedSurfaceFormat(pxInstance);
	xSwapChaincreateInfo.imageColorSpace = Instance_GetPreferedSurfaceColorSpace(pxInstance);
	xSwapChaincreateInfo.imageExtent.width = NativeWindow_GetWidth();
	xSwapChaincreateInfo.imageExtent.height = NativeWindow_GetHeight();
	xSwapChaincreateInfo.imageArrayLayers = 1;
	xSwapChaincreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	xSwapChaincreateInfo.preTransform = xSurfaceCapabilities.currentTransform;
	xSwapChaincreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	xSwapChaincreateInfo.presentMode = Instance_GetPreferedPresentMode(pxInstance);
	xSwapChaincreateInfo.clipped = VK_TRUE;
	xSwapChaincreateInfo.oldSwapchain = VK_NULL_HANDLE;

	int32_t nGraphicsQueueIndex = Instance_GetGraphicQueueIndex(pxInstance);
	int32_t nPresentQueueIndex = Instance_GetPresentQueueIndex(pxInstance);

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

	VK_CHECK(vkCreateSwapchainKHR(Instance_GetDevice(pxInstance), &xSwapChaincreateInfo, 0, &pxSwapChain->xSwapChain));
}

static void SwapChain_CreateImageViews(struct xSwapChain_t* pxSwapChain, struct xInstance_t* pxInstance) {
	VK_CHECK(vkGetSwapchainImagesKHR(Instance_GetDevice(pxInstance), pxSwapChain->xSwapChain, &pxSwapChain->nImageCount, 0));
	pxSwapChain->pxImages = (VkImage*)malloc(sizeof(VkImage) * pxSwapChain->nImageCount);
	pxSwapChain->pxImageViews = (VkImageView*)malloc(sizeof(VkImageView) * pxSwapChain->nImageCount);
	VK_CHECK(vkGetSwapchainImagesKHR(Instance_GetDevice(pxInstance), pxSwapChain->xSwapChain, &pxSwapChain->nImageCount, pxSwapChain->pxImages));

	VkImageViewCreateInfo xImageViewCreateInfo;

	for (uint32_t i = 0; i < pxSwapChain->nImageCount; ++i) {
		memset(&xImageViewCreateInfo, 0, sizeof(xImageViewCreateInfo));
		xImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		xImageViewCreateInfo.image = pxSwapChain->pxImages[i];
		xImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		xImageViewCreateInfo.format = Instance_GetPreferedSurfaceFormat(pxInstance);
		xImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		xImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		xImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		xImageViewCreateInfo.subresourceRange.levelCount = 1;
		xImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		xImageViewCreateInfo.subresourceRange.layerCount = 1;

		VK_CHECK(vkCreateImageView(Instance_GetDevice(pxInstance), &xImageViewCreateInfo, 0, &pxSwapChain->pxImageViews[i]));
	}
}

static void SwapChain_CreateRenderPass(struct xSwapChain_t* pxSwapChain, struct xInstance_t* pxInstance) {
	VkAttachmentDescription xColorAttachmentDesc;
	memset(&xColorAttachmentDesc, 0, sizeof(xColorAttachmentDesc));
	xColorAttachmentDesc.format = Instance_GetPreferedSurfaceFormat(pxInstance);
	xColorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	xColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	xColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	xColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	xColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	xColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	xColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference xColorAttachmentRef;
	memset(&xColorAttachmentRef, 0, sizeof(xColorAttachmentRef));
	xColorAttachmentRef.attachment = 0;
	xColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription xSubpassDesc;
	memset(&xSubpassDesc, 0, sizeof(xSubpassDesc));
	xSubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	xSubpassDesc.colorAttachmentCount = 1;
	xSubpassDesc.pColorAttachments = &xColorAttachmentRef;

	VkSubpassDependency xSubpassDependency;
	memset(&xSubpassDependency, 0, sizeof(xSubpassDependency));
	xSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	xSubpassDependency.dstSubpass = 0;
	xSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	xSubpassDependency.srcAccessMask = 0;
	xSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	xSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo xRenderPassCreateInfo;
	memset(&xRenderPassCreateInfo, 0, sizeof(xRenderPassCreateInfo));
	xRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	xRenderPassCreateInfo.attachmentCount = 1;
	xRenderPassCreateInfo.pAttachments = &xColorAttachmentDesc;
	xRenderPassCreateInfo.subpassCount = 1;
	xRenderPassCreateInfo.pSubpasses = &xSubpassDesc;
	xRenderPassCreateInfo.dependencyCount = 1;
	xRenderPassCreateInfo.pDependencies = &xSubpassDependency;

	VK_CHECK(vkCreateRenderPass(Instance_GetDevice(pxInstance), &xRenderPassCreateInfo, 0, &pxSwapChain->xRenderPass));
}

static void SwapChain_CreateFrameBuffers(struct xSwapChain_t* pxSwapChain, struct xInstance_t* pxInstance) {
	pxSwapChain->pxFrameBuffer = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * pxSwapChain->nImageCount);

	for (uint32_t i = 0; i < pxSwapChain->nImageCount; ++i) {
		VkImageView axAttachments[] = { pxSwapChain->pxImageViews[i] };

		VkFramebufferCreateInfo xFramebufferCreateInfo;
		memset(&xFramebufferCreateInfo, 0, sizeof(xFramebufferCreateInfo));
		xFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		xFramebufferCreateInfo.renderPass = pxSwapChain->xRenderPass;
		xFramebufferCreateInfo.attachmentCount = ARRAY_LENGTH(axAttachments);
		xFramebufferCreateInfo.pAttachments = axAttachments;
		xFramebufferCreateInfo.width = NativeWindow_GetWidth();
		xFramebufferCreateInfo.height = NativeWindow_GetHeight();
		xFramebufferCreateInfo.layers = 1;

		VK_CHECK(vkCreateFramebuffer(Instance_GetDevice(pxInstance), &xFramebufferCreateInfo, 0, &pxSwapChain->pxFrameBuffer[i]));
	}
}

struct xSwapChain_t* SwapChain_Alloc(struct xInstance_t* pxInstance) {
	struct xSwapChain_t* pxSwapChain = (struct xSwapChain_t*)calloc(1, sizeof(struct xSwapChain_t));

	SwapChain_CreateSwapChain(pxSwapChain, pxInstance);
	SwapChain_CreateImageViews(pxSwapChain, pxInstance);
	SwapChain_CreateRenderPass(pxSwapChain, pxInstance);
	SwapChain_CreateFrameBuffers(pxSwapChain, pxInstance);

	return pxSwapChain;
}

void SwapChain_Free(struct xSwapChain_t* pxSwapChain, struct xInstance_t* pxInstance) {
	for (uint32_t i = 0; i < SwapChain_GetImageCount(pxSwapChain); ++i) {
		vkDestroyFramebuffer(Instance_GetDevice(pxInstance), pxSwapChain->pxFrameBuffer[i], 0);
	}

	free(pxSwapChain->pxFrameBuffer);

	vkDestroyRenderPass(Instance_GetDevice(pxInstance), pxSwapChain->xRenderPass, 0);

	for (uint32_t i = 0; i < pxSwapChain->nImageCount; ++i) {
		vkDestroyImageView(Instance_GetDevice(pxInstance), pxSwapChain->pxImageViews[i], 0);
	}

	free(pxSwapChain->pxImageViews);
	free(pxSwapChain->pxImages);

	vkDestroySwapchainKHR(Instance_GetDevice(pxInstance), pxSwapChain->xSwapChain, 0);

	free(pxSwapChain);
}

VkSwapchainKHR SwapChain_GetSwapChain(struct xSwapChain_t* pxSwapChain) {
	return pxSwapChain->xSwapChain;
}

uint32_t SwapChain_GetImageCount(struct xSwapChain_t* pxSwapChain) {
	return pxSwapChain->nImageCount;
}

VkRenderPass SwapChain_GetRenderPass(struct xSwapChain_t* pxSwapChain) {
	return pxSwapChain->xRenderPass;
}

VkFramebuffer SwapChain_GetFrameBuffer(struct xSwapChain_t* pxSwapChain, uint32_t nIndex) {
	return pxSwapChain->pxFrameBuffer[nIndex];
}
