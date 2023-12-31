
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <vulkan/common.h>
#include <vulkan/instance.h>
#include <vulkan/image.h>

struct xImage_t {
	uint64_t wSize;
	uint32_t nWidth;
	uint32_t nHeight;
	VkFormat xFormat;
	VkImage xImage;
	VkDeviceMemory xDeviceMemory;
	VkImageView xImageView;
	VkSampler xSampler;
	void* pMappedData;
};

struct xImage_t* Image_Alloc(struct xInstance_t* pxInstance, uint32_t nWidth, uint32_t nHeight, VkImageUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties, VkFormat xFormat, VkImageTiling xTiling, VkFilter xFilter) {
	struct xImage_t* pxImage = (struct xImage_t*)calloc(1, sizeof(struct xImage_t));

	pxImage->wSize = nWidth * nHeight * 4;
	pxImage->nWidth = nWidth;
	pxImage->nHeight = nHeight;
	pxImage->xFormat = xFormat;

	int32_t nGraphicAndComputeQueueIndex = Instance_GetGraphicAndComputeQueueIndex(pxInstance);
	int32_t nPresentQueueIndex = Instance_GetPresentQueueIndex(pxInstance);

	uint32_t anQueueFamilies[] = { nGraphicAndComputeQueueIndex, nPresentQueueIndex };

	VkImageCreateInfo xImageCreateInfo;
	memset(&xImageCreateInfo, 0, sizeof(xImageCreateInfo));
	xImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	xImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	xImageCreateInfo.extent.width = nWidth;
	xImageCreateInfo.extent.height = nHeight;
	xImageCreateInfo.extent.depth = 1;
	xImageCreateInfo.mipLevels = 1;
	xImageCreateInfo.arrayLayers = 1;
	xImageCreateInfo.format = xFormat;
	xImageCreateInfo.tiling = xTiling;
	xImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	xImageCreateInfo.usage = xUsage;
	xImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	xImageCreateInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	xImageCreateInfo.queueFamilyIndexCount = ARRAY_LENGTH(anQueueFamilies);
	xImageCreateInfo.pQueueFamilyIndices = anQueueFamilies;

	VK_CHECK(vkCreateImage(Instance_GetDevice(pxInstance), &xImageCreateInfo, 0, &pxImage->xImage));

	VkMemoryRequirements xMemoryRequirements;
	vkGetImageMemoryRequirements(Instance_GetDevice(pxInstance), pxImage->xImage, &xMemoryRequirements);

	VkMemoryAllocateInfo xMemoryAllocateInfo;
	memset(&xMemoryAllocateInfo, 0, sizeof(xMemoryAllocateInfo));
	xMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	xMemoryAllocateInfo.allocationSize = xMemoryRequirements.size;
	xMemoryAllocateInfo.memoryTypeIndex = Instance_CheckMemoryType(pxInstance, xMemoryRequirements.memoryTypeBits, xMemoryProperties);

	VK_CHECK(vkAllocateMemory(Instance_GetDevice(pxInstance), &xMemoryAllocateInfo, 0, &pxImage->xDeviceMemory));
	VK_CHECK(vkBindImageMemory(Instance_GetDevice(pxInstance), pxImage->xImage, pxImage->xDeviceMemory, 0));

	VkImageViewCreateInfo xImageViewCreateInfo;
	memset(&xImageViewCreateInfo, 0, sizeof(xImageViewCreateInfo));
	xImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	xImageViewCreateInfo.image = pxImage->xImage;
	xImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	xImageViewCreateInfo.format = xFormat;
	xImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	xImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	xImageViewCreateInfo.subresourceRange.levelCount = 1;
	xImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	xImageViewCreateInfo.subresourceRange.layerCount = 1;

	VK_CHECK(vkCreateImageView(Instance_GetDevice(pxInstance), &xImageViewCreateInfo, 0, &pxImage->xImageView));

	VkPhysicalDeviceProperties xPhysicalDeviceProperties;
	vkGetPhysicalDeviceProperties(Instance_GetPhysicalDevice(pxInstance), &xPhysicalDeviceProperties);

	VkSamplerCreateInfo xSamplerCreateInfo;
	memset(&xSamplerCreateInfo, 0, sizeof(xSamplerCreateInfo));
	xSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	xSamplerCreateInfo.magFilter = xFilter;
	xSamplerCreateInfo.minFilter = xFilter;
	xSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	xSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	xSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	xSamplerCreateInfo.anisotropyEnable = VK_TRUE;
	xSamplerCreateInfo.maxAnisotropy = xPhysicalDeviceProperties.limits.maxSamplerAnisotropy;
	xSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	xSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	xSamplerCreateInfo.compareEnable = VK_FALSE;
	xSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	xSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	xSamplerCreateInfo.mipLodBias = 0.0F;
	xSamplerCreateInfo.minLod = 0.0F;
	xSamplerCreateInfo.maxLod = 0.0F;

	VK_CHECK(vkCreateSampler(Instance_GetDevice(pxInstance), &xSamplerCreateInfo, 0, &pxImage->xSampler));

	return pxImage;
}

void Image_Free(struct xImage_t* pxImage, struct xInstance_t* pxInstance) {
	if (pxImage->pMappedData) {
		vkUnmapMemory(Instance_GetDevice(pxInstance), pxImage->xDeviceMemory);
	}

	vkDestroySampler(Instance_GetDevice(pxInstance), pxImage->xSampler, 0);
	vkDestroyImageView(Instance_GetDevice(pxInstance), pxImage->xImageView, 0);
	vkDestroyImage(Instance_GetDevice(pxInstance), pxImage->xImage, 0);
	vkFreeMemory(Instance_GetDevice(pxInstance), pxImage->xDeviceMemory, 0);

	free(pxImage);
}

uint64_t Image_GetSize(struct xImage_t* pxImage) {
	return pxImage->wSize;
}

uint32_t Image_GetWidth(struct xImage_t* pxImage) {
	return pxImage->nWidth;
}

uint32_t Image_GetHeight(struct xImage_t* pxImage) {
	return pxImage->nHeight;
}

VkFormat Image_GetFormat(struct xImage_t* pxImage) {
	return pxImage->xFormat;
}

VkImage Image_GetImage(struct xImage_t* pxImage) {
	return pxImage->xImage;
}

VkDeviceMemory Image_GetDeviceMemory(struct xImage_t* pxImage) {
	return pxImage->xDeviceMemory;
}

VkImageView Image_GetImageView(struct xImage_t* pxImage) {
	return pxImage->xImageView;
}

VkSampler Image_GetSampler(struct xImage_t* pxImage) {
	return pxImage->xSampler;
}

void* Image_GetMappedData(struct xImage_t* pxImage) {
	return pxImage->pMappedData;
}

void* Image_GetMappedDataRef(struct xImage_t* pxImage) {
	return &pxImage->pMappedData;
}

void Image_Map(struct xImage_t* pxImage, struct xInstance_t* pxInstance) {
	VK_CHECK(vkMapMemory(Instance_GetDevice(pxInstance), pxImage->xDeviceMemory, 0, pxImage->wSize, 0, &pxImage->pMappedData));
}

void Image_UnMap(struct xImage_t* pxImage, struct xInstance_t* pxInstance) {
	vkUnmapMemory(Instance_GetDevice(pxInstance), pxImage->xDeviceMemory);
	pxImage->pMappedData = 0;
}

void Image_SetTo(struct xImage_t* pxImage, void* pData, uint64_t wSize) {
	memcpy(pxImage->pMappedData, pData, wSize);
}

void Image_LayoutTransition(struct xImage_t* pxImage, VkCommandBuffer xCommandBuffer, VkImageLayout xOldLayout, VkImageLayout xNewLayout) {
	VkImageMemoryBarrier xImageMemoryBarrier;
	memset(&xImageMemoryBarrier, 0, sizeof(xImageMemoryBarrier));
	xImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	xImageMemoryBarrier.oldLayout = xOldLayout;
	xImageMemoryBarrier.newLayout = xNewLayout;
	xImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	xImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	xImageMemoryBarrier.image = pxImage->xImage;
	xImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	xImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	xImageMemoryBarrier.subresourceRange.levelCount = 1;
	xImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	xImageMemoryBarrier.subresourceRange.layerCount = 1;
	xImageMemoryBarrier.srcAccessMask = 0;
	xImageMemoryBarrier.dstAccessMask = 0;

	VkPipelineStageFlags xPipelineSourceStageFlags = VK_PIPELINE_STAGE_NONE;
	VkPipelineStageFlags xPipelineDestinationStageFlags = VK_PIPELINE_STAGE_NONE;

	if ((xOldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (xNewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)) {
		xImageMemoryBarrier.srcAccessMask = 0;
		xImageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		xPipelineSourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		xPipelineDestinationStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if ((xOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (xNewLayout == VK_IMAGE_LAYOUT_UNDEFINED)) {
		xImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		xImageMemoryBarrier.dstAccessMask = 0;

		xPipelineSourceStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		xPipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	} else if ((xOldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) && (xNewLayout == VK_IMAGE_LAYOUT_GENERAL)) {
		xImageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		xImageMemoryBarrier.dstAccessMask = 0;

		xPipelineSourceStageFlags = VK_PIPELINE_STAGE_TRANSFER_BIT;
		xPipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	} else if ((xOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) && (xNewLayout == VK_IMAGE_LAYOUT_UNDEFINED)) {
		xImageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		xImageMemoryBarrier.dstAccessMask = 0;

		xPipelineSourceStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		xPipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	} else if ((xOldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (xNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)) {
		xImageMemoryBarrier.srcAccessMask = 0;
		xImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		xPipelineSourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		xPipelineDestinationStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else if ((xOldLayout == VK_IMAGE_LAYOUT_GENERAL) && (xNewLayout == VK_IMAGE_LAYOUT_UNDEFINED)) {
		xImageMemoryBarrier.srcAccessMask = 0;
		xImageMemoryBarrier.dstAccessMask = 0;

		xPipelineSourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		xPipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	} else if ((xOldLayout == VK_IMAGE_LAYOUT_UNDEFINED) && (xNewLayout == VK_IMAGE_LAYOUT_GENERAL)) {
		xImageMemoryBarrier.srcAccessMask = 0;
		xImageMemoryBarrier.dstAccessMask = 0;

		xPipelineSourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		xPipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	} else if ((xOldLayout == VK_IMAGE_LAYOUT_GENERAL) && (xNewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)) {
		xImageMemoryBarrier.srcAccessMask = 0;
		xImageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		xPipelineSourceStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		xPipelineDestinationStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else if ((xOldLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) && (xNewLayout == VK_IMAGE_LAYOUT_GENERAL)) {
		xImageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		xImageMemoryBarrier.dstAccessMask = 0;

		xPipelineSourceStageFlags = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		xPipelineDestinationStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	} 

	vkCmdPipelineBarrier(xCommandBuffer, xPipelineSourceStageFlags, xPipelineDestinationStageFlags, 0, 0, 0, 0, 0, 1, &xImageMemoryBarrier);
}
