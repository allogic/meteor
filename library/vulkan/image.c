
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <vulkan/instance.h>
#include <vulkan/image.h>
#include <vulkan/command.h>

struct xImage_t {
	uint64_t wSize;
	VkImage xImage;
	VkDeviceMemory xDeviceMemory;
	void* pMappedData;
};

struct xImage_t* Image_Alloc(struct xInstance_t* pxInstance, uint32_t nWidth, uint32_t nHeight, VkImageUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties, VkFormat xFormat, VkImageTiling xTiling) {
	struct xImage_t* pxImage = (struct xImage_t*)calloc(1, sizeof(struct xImage_t));

	VkImageCreateInfo xVkImageCreateInfo;
	memset(&xVkImageCreateInfo, 0, sizeof(xVkImageCreateInfo));
	xVkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	xVkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	xVkImageCreateInfo.extent.width = nWidth;
	xVkImageCreateInfo.extent.height = nHeight;
	xVkImageCreateInfo.extent.depth = 1;
	xVkImageCreateInfo.mipLevels = 1;
	xVkImageCreateInfo.arrayLayers = 1;
	xVkImageCreateInfo.format = xFormat;
	xVkImageCreateInfo.tiling = xTiling;
	xVkImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	xVkImageCreateInfo.usage = xUsage;
	xVkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	xVkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateImage(Instance_GetDevice(pxInstance), &xVkImageCreateInfo, 0, &pxImage->xImage));

	VkMemoryRequirements xMemoryRequirements;
	vkGetImageMemoryRequirements(Instance_GetDevice(pxInstance), pxImage->xImage, &xMemoryRequirements);

	VkMemoryAllocateInfo xMemoryAllocateInfo;
	memset(&xMemoryAllocateInfo, 0, sizeof(xMemoryAllocateInfo));
	xMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	xMemoryAllocateInfo.allocationSize = xMemoryRequirements.size;
	xMemoryAllocateInfo.memoryTypeIndex = Instance_CheckMemoryType(pxInstance, xMemoryRequirements.memoryTypeBits, xMemoryProperties);

	VK_CHECK(vkAllocateMemory(Instance_GetDevice(pxInstance), &xMemoryAllocateInfo, 0, &pxImage->xDeviceMemory));
	VK_CHECK(vkBindImageMemory(Instance_GetDevice(pxInstance), pxImage->xImage, pxImage->xDeviceMemory, 0));

	return pxImage;
}

void Image_Free(struct xImage_t* pxImage, struct xInstance_t* pxInstance) {
	if (pxImage->pMappedData) {
		vkUnmapMemory(Instance_GetDevice(pxInstance), pxImage->xDeviceMemory);
	}

	vkDestroyImage(Instance_GetDevice(pxInstance), pxImage->xImage, 0);
	vkFreeMemory(Instance_GetDevice(pxInstance), pxImage->xDeviceMemory, 0);

	free(pxImage);
}

VkImage Image_GetImage(struct xImage_t* pxImage) {
	return pxImage->xImage;
}

VkDeviceMemory Image_GetDeviceMemory(struct xImage_t* pxImage) {
	return pxImage->xDeviceMemory;
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

void Image_LayoutTransition(struct xImage_t* pxImage, struct xInstance_t* pxInstance, VkFormat xFormat, VkImageLayout xOldLayout, VkImageLayout xNewLayout) {
	VkCommandBuffer xCommandBuffer = Command_BeginSingleTimeCommands(pxInstance);

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

	vkCmdPipelineBarrier(xCommandBuffer, 0, 0, 0, 0, 0, 0, 0, 1, &xImageMemoryBarrier);

	Command_EndSingleTimeCommands(pxInstance, xCommandBuffer);
}
