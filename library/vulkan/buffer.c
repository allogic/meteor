#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <vulkan/instance.h>
#include <vulkan/buffer.h>
#include <vulkan/image.h>
#include <vulkan/command.h>

struct xBuffer_t {
	uint64_t wSize;
	VkBuffer xBuffer;
	VkDeviceMemory xDeviceMemory;
	void* pMappedData;
};

struct xBuffer_t* Buffer_Alloc(struct xInstance_t* pxInstance, uint64_t wSize, VkBufferUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties) {
	struct xBuffer_t* pxBuffer = (struct xBuffer_t*)calloc(1, sizeof(struct xBuffer_t));

	pxBuffer->wSize = wSize;

	VkBufferCreateInfo xBufferCreateInfo;
	memset(&xBufferCreateInfo, 0, sizeof(xBufferCreateInfo));
	xBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	xBufferCreateInfo.size = wSize;
	xBufferCreateInfo.usage = xUsage;
	xBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateBuffer(Instance_GetDevice(pxInstance), &xBufferCreateInfo, 0, &pxBuffer->xBuffer));

	VkMemoryRequirements xMemoryRequirements;
	vkGetBufferMemoryRequirements(Instance_GetDevice(pxInstance), pxBuffer->xBuffer, &xMemoryRequirements);

	VkMemoryAllocateInfo xMemoryAllocateInfo;
	memset(&xMemoryAllocateInfo, 0, sizeof(xMemoryAllocateInfo));
	xMemoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	xMemoryAllocateInfo.allocationSize = xMemoryRequirements.size;
	xMemoryAllocateInfo.memoryTypeIndex = Instance_CheckMemoryType(pxInstance, xMemoryRequirements.memoryTypeBits, xMemoryProperties);

	VK_CHECK(vkAllocateMemory(Instance_GetDevice(pxInstance), &xMemoryAllocateInfo, 0, &pxBuffer->xDeviceMemory));
	VK_CHECK(vkBindBufferMemory(Instance_GetDevice(pxInstance), pxBuffer->xBuffer, pxBuffer->xDeviceMemory, 0));

	return pxBuffer;
}

void Buffer_Free(struct xBuffer_t* pxBuffer, struct xInstance_t* pxInstance) {
	if (pxBuffer->pMappedData) {
		vkUnmapMemory(Instance_GetDevice(pxInstance), pxBuffer->xDeviceMemory);
	}

	vkDestroyBuffer(Instance_GetDevice(pxInstance), pxBuffer->xBuffer, 0);
	vkFreeMemory(Instance_GetDevice(pxInstance), pxBuffer->xDeviceMemory, 0);

	free(pxBuffer);
}

uint64_t Buffer_GetSize(struct xBuffer_t* pxBuffer) {
	return pxBuffer->wSize;
}

VkBuffer Buffer_GetBuffer(struct xBuffer_t* pxBuffer) {
	return pxBuffer->xBuffer;
}

VkDeviceMemory Buffer_GetDeviceMemory(struct xBuffer_t* pxBuffer) {
	return pxBuffer->xDeviceMemory;
}

void* Buffer_GetMappedData(struct xBuffer_t* pxBuffer) {
	return pxBuffer->pMappedData;
}

void* Buffer_GetMappedDataRef(struct xBuffer_t* pxBuffer) {
	return &pxBuffer->pMappedData;
}

void Buffer_Map(struct xBuffer_t* pxBuffer, struct xInstance_t* pxInstance) {
	VK_CHECK(vkMapMemory(Instance_GetDevice(pxInstance), pxBuffer->xDeviceMemory, 0, pxBuffer->wSize, 0, &pxBuffer->pMappedData));
}

void Buffer_UnMap(struct xBuffer_t* pxBuffer, struct xInstance_t* pxInstance) {
	vkUnmapMemory(Instance_GetDevice(pxInstance), pxBuffer->xDeviceMemory);
	pxBuffer->pMappedData = 0;
}

void Buffer_CopyToBuffer(struct xInstance_t* pxInstance, struct xBuffer_t* pxSourceBuffer, struct xBuffer_t* pxDestinationBuffer, uint64_t wSize) {
	VkCommandBuffer xCommandBuffer = Command_BeginSingleTimeCommands(pxInstance);

	VkBufferCopy xBufferCopy;
	memset(&xBufferCopy, 0, sizeof(xBufferCopy));
	xBufferCopy.size = wSize;
	vkCmdCopyBuffer(xCommandBuffer, pxSourceBuffer->xBuffer, pxDestinationBuffer->xBuffer, 1, &xBufferCopy);

	Command_EndSingleTimeCommands(pxInstance, xCommandBuffer);
}

void Buffer_CopyToImage(struct xInstance_t* pxInstance, struct xBuffer_t* pxBuffer, struct xImage_t* pxImage, uint32_t nWidth, uint32_t nHeight) {
	VkCommandBuffer xCommandBuffer = Command_BeginSingleTimeCommands(pxInstance);

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

	vkCmdCopyBufferToImage(xCommandBuffer, pxBuffer->xBuffer, Image_GetImage(pxImage), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &xBufferImageCopy);

	Command_EndSingleTimeCommands(pxInstance, xCommandBuffer);
}

void Buffer_Copy(struct xBuffer_t* pxBuffer, void* pData, uint64_t wSize) {
	memcpy(pxBuffer->pMappedData, pData, wSize);
}
