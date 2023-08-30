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
	VkRenderPass xRenderPass;
	VkFramebuffer* pxFrameBuffer;
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

static void VkSwapChain_CreateRenderPass(struct xVkSwapChain_t* pxVkSwapChain, struct xVkInstance_t* pxVkInstance) {
	VkAttachmentDescription xColorAttachmentDesc;
	memset(&xColorAttachmentDesc, 0, sizeof(xColorAttachmentDesc));
	xColorAttachmentDesc.format = VkInstance_GetPreferedSurfaceFormat(pxVkInstance);
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

	VK_CHECK(vkCreateRenderPass(VkInstance_GetDevice(pxVkInstance), &xRenderPassCreateInfo, 0, &pxVkSwapChain->xRenderPass));
}

static void VkSwapChain_CreateFrameBuffers(struct xVkSwapChain_t* pxVkSwapChain, struct xVkInstance_t* pxVkInstance) {
	pxVkSwapChain->pxFrameBuffer = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * pxVkSwapChain->nImageCount);

	for (uint32_t i = 0; i < pxVkSwapChain->nImageCount; ++i) {
		VkImageView axAttachments[] = { pxVkSwapChain->pxImageViews[i] };

		VkFramebufferCreateInfo xFramebufferCreateInfo;
		memset(&xFramebufferCreateInfo, 0, sizeof(xFramebufferCreateInfo));
		xFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		xFramebufferCreateInfo.renderPass = pxVkSwapChain->xRenderPass;
		xFramebufferCreateInfo.attachmentCount = ARRAY_LENGTH(axAttachments);
		xFramebufferCreateInfo.pAttachments = axAttachments;
		xFramebufferCreateInfo.width = NativeWindow_GetWidth();
		xFramebufferCreateInfo.height = NativeWindow_GetHeight();
		xFramebufferCreateInfo.layers = 1;

		VK_CHECK(vkCreateFramebuffer(VkInstance_GetDevice(pxVkInstance), &xFramebufferCreateInfo, 0, &pxVkSwapChain->pxFrameBuffer[i]));
	}
}

struct xVkSwapChain_t* VkSwapChain_Alloc(struct xVkInstance_t* pxVkInstance) {
	struct xVkSwapChain_t* pxVkSwapChain = (struct xVkSwapChain_t*)calloc(1, sizeof(struct xVkSwapChain_t));

	VkSwapChain_CreateSwapChain(pxVkSwapChain, pxVkInstance);
	VkSwapChain_CreateImageViews(pxVkSwapChain, pxVkInstance);
	VkSwapChain_CreateRenderPass(pxVkSwapChain, pxVkInstance);
	VkSwapChain_CreateFrameBuffers(pxVkSwapChain, pxVkInstance);

	return pxVkSwapChain;
}

void VkSwapChain_Free(struct xVkSwapChain_t* pxVkSwapChain, struct xVkInstance_t* pxVkInstance) {
	for (uint32_t i = 0; i < VkSwapChain_GetImageCount(pxVkSwapChain); ++i) {
		vkDestroyFramebuffer(VkInstance_GetDevice(pxVkInstance), pxVkSwapChain->pxFrameBuffer[i], 0);
	}

	free(pxVkSwapChain->pxFrameBuffer);

	vkDestroyRenderPass(VkInstance_GetDevice(pxVkInstance), pxVkSwapChain->xRenderPass, 0);

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

VkRenderPass VkSwapChain_GetRenderPass(struct xVkSwapChain_t* pxVkSwapChain) {
	return pxVkSwapChain->xRenderPass;
}

VkFramebuffer VkSwapChain_GetFrameBuffer(struct xVkSwapChain_t* pxVkSwapChain, uint32_t nIndex) {
	return pxVkSwapChain->pxFrameBuffer[nIndex];
}
