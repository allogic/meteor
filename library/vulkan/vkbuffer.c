#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkbuffer.h>
#include <vulkan/vkimage.h>
#include <vulkan/vkcommand.h>

struct xVkBuffer_t {
	uint64_t wSize;
	VkBuffer xBuffer;
	VkDeviceMemory xDeviceMemory;
	void* pMappedData;
};

struct xVkBuffer_t* VkBuffer_Alloc(struct xVkInstance_t* pxVkInstance, uint64_t wSize, VkBufferUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties) {
	struct xVkBuffer_t* pxVkBuffer = (struct xVkBuffer_t*)calloc(1, sizeof(struct xVkBuffer_t));

	pxVkBuffer->wSize = wSize;

	VkBufferCreateInfo xBufferCreateInfo;
	memset(&xBufferCreateInfo, 0, sizeof(xBufferCreateInfo));
	xBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	xBufferCreateInfo.size = wSize;
	xBufferCreateInfo.usage = xUsage;
	xBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateBuffer(VkInstance_GetDevice(pxVkInstance), &xBufferCreateInfo, 0, &pxVkBuffer->xBuffer));

	VkMemoryRequirements xMemoryRequirements;
	vkGetBufferMemoryRequirements(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xBuffer, &xMemoryRequirements);

	VkMemoryAllocateInfo xMemoryAllocateInfo;
	memset(&xMemoryAllocateInfo, 0, sizeof(xMemoryAllocateInfo));
	xMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	xMemoryAllocateInfo.allocationSize = xMemoryRequirements.size;
	xMemoryAllocateInfo.memoryTypeIndex = VkInstance_CheckMemoryType(pxVkInstance, xMemoryRequirements.memoryTypeBits, xMemoryProperties);

	VK_CHECK(vkAllocateMemory(VkInstance_GetDevice(pxVkInstance), &xMemoryAllocateInfo, 0, &pxVkBuffer->xDeviceMemory));
	VK_CHECK(vkBindBufferMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xBuffer, pxVkBuffer->xDeviceMemory, 0));

	return pxVkBuffer;
}

void VkBuffer_Free(struct xVkBuffer_t* pxVkBuffer, struct xVkInstance_t* pxVkInstance) {
	if (pxVkBuffer->pMappedData) {
		vkUnmapMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xDeviceMemory);
	}

	vkDestroyBuffer(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xBuffer, 0);
	vkFreeMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xDeviceMemory, 0);

	free(pxVkBuffer);
}

VkBuffer VkBuffer_GetBuffer(struct xVkBuffer_t* pxVkBuffer) {
	return pxVkBuffer->xBuffer;
}

VkDeviceMemory VkBuffer_GetDeviceMemory(struct xVkBuffer_t* pxVkBuffer) {
	return pxVkBuffer->xDeviceMemory;
}

void* VkBuffer_GetMappedData(struct xVkBuffer_t* pxVkBuffer) {
	return pxVkBuffer->pMappedData;
}

void* VkBuffer_GetMappedDataRef(struct xVkBuffer_t* pxVkBuffer) {
	return &pxVkBuffer->pMappedData;
}

void VkBuffer_Map(struct xVkBuffer_t* pxVkBuffer, struct xVkInstance_t* pxVkInstance) {
	VK_CHECK(vkMapMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xDeviceMemory, 0, pxVkBuffer->wSize, 0, &pxVkBuffer->pMappedData));
}

void VkBuffer_UnMap(struct xVkBuffer_t* pxVkBuffer, struct xVkInstance_t* pxVkInstance) {
	vkUnmapMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xDeviceMemory);
	pxVkBuffer->pMappedData = 0;
}

void VkBuffer_CopyToBuffer(struct xVkInstance_t* pxVkInstance, struct xVkBuffer_t* pxVkSourceBuffer, struct xVkBuffer_t* pxVkDestinationBuffer, uint64_t wSize) {
	VkCommandBuffer xCommandBuffer = VkCommand_BeginSingleTimeCommands(pxVkInstance);

	VkBufferCopy xBufferCopy;
	memset(&xBufferCopy, 0, sizeof(xBufferCopy));
	xBufferCopy.size = wSize;
	vkCmdCopyBuffer(xCommandBuffer, pxVkSourceBuffer->xBuffer, pxVkDestinationBuffer->xBuffer, 1, &xBufferCopy);

	VkCommand_EndSingleTimeCommands(pxVkInstance, xCommandBuffer);
}

void VkBuffer_CopyToImage(struct xVkInstance_t* pxVkInstance, struct xVkBuffer_t* pxVkBuffer, struct xVkImage_t* pxVkImage, uint32_t nWidth, uint32_t nHeight) {
	VkCommandBuffer xCommandBuffer = VkCommand_BeginSingleTimeCommands(pxVkInstance);

	VkBufferImageCopy xBufferImageCopy;
	memset(&xBufferImageCopy, 0, sizeof(xBufferImageCopy));
	xBufferImageCopy.bufferOffset = 0;
	xBufferImageCopy.bufferRowLength = 0;
	xBufferImageCopy.bufferImageHeight = 0;
	xBufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	xBufferImageCopy.imageSubresource.mipLevel = 0;
	xBufferImageCopy.imageSubresource.baseArrayLayer = 0;
	xBufferImageCopy.imageSubresource.layerCount = 1;
	xBufferImageCopy.imageOffset.x = 0;
	xBufferImageCopy.imageOffset.y = 0;
	xBufferImageCopy.imageOffset.z = 0;
	xBufferImageCopy.imageExtent.width = nWidth;
	xBufferImageCopy.imageExtent.height = nHeight;
	xBufferImageCopy.imageExtent.depth = 1;

	vkCmdCopyBufferToImage(xCommandBuffer, pxVkBuffer->xBuffer, VkImage_GetImage(pxVkImage), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &xBufferImageCopy);

	VkCommand_EndSingleTimeCommands(pxVkInstance, xCommandBuffer);
}

void VkBuffer_Copy(struct xVkBuffer_t* pxVkBuffer, void* pData, uint64_t wSize) {
	memcpy(pxVkBuffer->pMappedData, pData, wSize);
}
