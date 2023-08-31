
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkimage.h>
#include <vulkan/vkcommand.h>

struct xVkImage_t {
	uint64_t wSize;
	VkImage xImage;
	VkDeviceMemory xDeviceMemory;
	void* pMappedData;
};

struct xVkImage_t* VkImage_Alloc(struct xVkInstance_t* pxVkInstance, uint32_t nWidth, uint32_t nHeight, VkImageUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties, VkFormat xFormat, VkImageTiling xTiling) {
	struct xVkImage_t* pxVkImage = (struct xVkImage_t*)calloc(1, sizeof(struct xVkImage_t));

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

	VK_CHECK(vkCreateImage(VkInstance_GetDevice(pxVkInstance), &xVkImageCreateInfo, 0, &pxVkImage->xImage));

	VkMemoryRequirements xMemoryRequirements;
	vkGetImageMemoryRequirements(VkInstance_GetDevice(pxVkInstance), pxVkImage->xImage, &xMemoryRequirements);

	VkMemoryAllocateInfo xMemoryAllocateInfo;
	memset(&xMemoryAllocateInfo, 0, sizeof(xMemoryAllocateInfo));
	xMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	xMemoryAllocateInfo.allocationSize = xMemoryRequirements.size;
	xMemoryAllocateInfo.memoryTypeIndex = VkInstance_CheckMemoryType(pxVkInstance, xMemoryRequirements.memoryTypeBits, xMemoryProperties);

	VK_CHECK(vkAllocateMemory(VkInstance_GetDevice(pxVkInstance), &xMemoryAllocateInfo, 0, &pxVkImage->xDeviceMemory));
	VK_CHECK(vkBindImageMemory(VkInstance_GetDevice(pxVkInstance), pxVkImage->xImage, pxVkImage->xDeviceMemory, 0));

	return pxVkImage;
}

void VkImage_Free(struct xVkImage_t* pxVkImage, struct xVkInstance_t* pxVkInstance) {
	if (pxVkImage->pMappedData) {
		vkUnmapMemory(VkInstance_GetDevice(pxVkInstance), pxVkImage->xDeviceMemory);
	}

	vkDestroyImage(VkInstance_GetDevice(pxVkInstance), pxVkImage->xImage, 0);
	vkFreeMemory(VkInstance_GetDevice(pxVkInstance), pxVkImage->xDeviceMemory, 0);

	free(pxVkImage);
}

VkImage VkImage_GetImage(struct xVkImage_t* pxVkImage) {
	return pxVkImage->xImage;
}

VkDeviceMemory VkImage_GetDeviceMemory(struct xVkImage_t* pxVkImage) {
	return pxVkImage->xDeviceMemory;
}

void* VkImage_GetMappedData(struct xVkImage_t* pxVkImage) {
	return pxVkImage->pMappedData;
}

void* VkImage_GetMappedDataRef(struct xVkImage_t* pxVkImage) {
	return &pxVkImage->pMappedData;
}

void VkImage_Map(struct xVkImage_t* pxVkImage, struct xVkInstance_t* pxVkInstance) {
	VK_CHECK(vkMapMemory(VkInstance_GetDevice(pxVkInstance), pxVkImage->xDeviceMemory, 0, pxVkImage->wSize, 0, &pxVkImage->pMappedData));
}

void VkImage_UnMap(struct xVkImage_t* pxVkImage, struct xVkInstance_t* pxVkInstance) {
	vkUnmapMemory(VkInstance_GetDevice(pxVkInstance), pxVkImage->xDeviceMemory);
	pxVkImage->pMappedData = 0;
}

void VkImage_LayoutTransition(struct xVkImage_t* pxVkImage, struct xVkInstance_t* pxVkInstance, VkFormat xFormat, VkImageLayout xOldLayout, VkImageLayout xNewLayout) {
	VkCommandBuffer xCommandBuffer = VkCommand_BeginSingleTimeCommands(pxVkInstance);

	VkImageMemoryBarrier xImageMemoryBarrier;
	memset(&xImageMemoryBarrier, 0, sizeof(xImageMemoryBarrier));
	xImageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	xImageMemoryBarrier.oldLayout = xOldLayout;
	xImageMemoryBarrier.newLayout = xNewLayout;
	xImageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	xImageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	xImageMemoryBarrier.image = pxVkImage->xImage;
	xImageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	xImageMemoryBarrier.subresourceRange.baseMipLevel = 0;
	xImageMemoryBarrier.subresourceRange.levelCount = 1;
	xImageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
	xImageMemoryBarrier.subresourceRange.layerCount = 1;
	xImageMemoryBarrier.srcAccessMask = 0;
	xImageMemoryBarrier.dstAccessMask = 0;

	vkCmdPipelineBarrier(xCommandBuffer, 0, 0, 0, 0, 0, 0, 0, 1, &xImageMemoryBarrier);

	VkCommand_EndSingleTimeCommands(pxVkInstance, xCommandBuffer);
}
