
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

struct xImage_t* VkImage_Alloc(struct xInstance_t* pxInstance, uint32_t nWidth, uint32_t nHeight, VkImageUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties, VkFormat xFormat, VkImageTiling xTiling) {
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

	VK_CHECK(vkCreateImage(VkInstance_GetDevice(pxInstance), &xVkImageCreateInfo, 0, &pxImage->xImage));

	VkMemoryRequirements xMemoryRequirements;
	vkGetImageMemoryRequirements(VkInstance_GetDevice(pxInstance), pxImage->xImage, &xMemoryRequirements);

	VkMemoryAllocateInfo xMemoryAllocateInfo;
	memset(&xMemoryAllocateInfo, 0, sizeof(xMemoryAllocateInfo));
	xMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	xMemoryAllocateInfo.allocationSize = xMemoryRequirements.size;
	xMemoryAllocateInfo.memoryTypeIndex = VkInstance_CheckMemoryType(pxInstance, xMemoryRequirements.memoryTypeBits, xMemoryProperties);

	VK_CHECK(vkAllocateMemory(VkInstance_GetDevice(pxInstance), &xMemoryAllocateInfo, 0, &pxImage->xDeviceMemory));
	VK_CHECK(vkBindImageMemory(VkInstance_GetDevice(pxInstance), pxImage->xImage, pxImage->xDeviceMemory, 0));

	return pxImage;
}

void VkImage_Free(struct xImage_t* pxImage, struct xInstance_t* pxInstance) {
	if (pxImage->pMappedData) {
		vkUnmapMemory(VkInstance_GetDevice(pxInstance), pxImage->xDeviceMemory);
	}

	vkDestroyImage(VkInstance_GetDevice(pxInstance), pxImage->xImage, 0);
	vkFreeMemory(VkInstance_GetDevice(pxInstance), pxImage->xDeviceMemory, 0);

	free(pxImage);
}

VkImage VkImage_GetImage(struct xImage_t* pxImage) {
	return pxImage->xImage;
}

VkDeviceMemory VkImage_GetDeviceMemory(struct xImage_t* pxImage) {
	return pxImage->xDeviceMemory;
}

void* VkImage_GetMappedData(struct xImage_t* pxImage) {
	return pxImage->pMappedData;
}

void* VkImage_GetMappedDataRef(struct xImage_t* pxImage) {
	return &pxImage->pMappedData;
}

void VkImage_Map(struct xImage_t* pxImage, struct xInstance_t* pxInstance) {
	VK_CHECK(vkMapMemory(VkInstance_GetDevice(pxInstance), pxImage->xDeviceMemory, 0, pxImage->wSize, 0, &pxImage->pMappedData));
}

void VkImage_UnMap(struct xImage_t* pxImage, struct xInstance_t* pxInstance) {
	vkUnmapMemory(VkInstance_GetDevice(pxInstance), pxImage->xDeviceMemory);
	pxImage->pMappedData = 0;
}

void VkImage_LayoutTransition(struct xImage_t* pxImage, struct xInstance_t* pxInstance, VkFormat xFormat, VkImageLayout xOldLayout, VkImageLayout xNewLayout) {
	VkCommandBuffer xCommandBuffer = VkCommand_BeginSingleTimeCommands(pxInstance);

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

	VkCommand_EndSingleTimeCommands(pxInstance, xCommandBuffer);
}
