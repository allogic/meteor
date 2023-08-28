#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <macros.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkbuffer.h>

struct xVkBuffer_t {
	VkBuffer xBuffer;
	VkDeviceMemory xDeviceMemory;
};

static int32_t VkBuffer_CheckMemoryType(struct xVkInstance_t* pxVkInstance, uint32_t nTypeFilter, VkMemoryPropertyFlags xMemoryPropertyFlags) {
	VkPhysicalDeviceMemoryProperties xPhysicalDeviceMemoryProperties;
	vkGetPhysicalDeviceMemoryProperties(VkInstance_GetPhysicalDevice(pxVkInstance), &xPhysicalDeviceMemoryProperties);

	for (uint32_t i = 0; i < xPhysicalDeviceMemoryProperties.memoryTypeCount; ++i) {
		if ((nTypeFilter & (1 << i)) && ((xPhysicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & xMemoryPropertyFlags) == xMemoryPropertyFlags)) {
			return i;
		}
	}

	return -1;
}

struct xVkBuffer_t* VkBuffer_Alloc(struct xVkInstance_t* pxVkInstance, uint64_t wSize, VkBufferUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties) {
	struct xVkBuffer_t* pxVkBuffer = (struct xVkBuffer_t*)calloc(1, sizeof(struct xVkBuffer_t));

	VkBufferCreateInfo xBufferCreateInfo;
	memset(&xBufferCreateInfo, 0, sizeof(xBufferCreateInfo));
	xBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	xBufferCreateInfo.size = wSize;
	xBufferCreateInfo.usage = xUsage;
	xBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateBuffer(VkInstance_GetDevice(pxVkInstance), &xBufferCreateInfo, 0, &pxVkBuffer->xBuffer));

	VkMemoryRequirements xMemoryRequirements;
	vkGetBufferMemoryRequirements(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xBuffer, &xMemoryRequirements);

	VkMemoryAllocateInfo xMemoryAllocteInfo;
	memset(&xMemoryAllocteInfo, 0, sizeof(xMemoryAllocteInfo));
	xMemoryAllocteInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	xMemoryAllocteInfo.allocationSize = xMemoryRequirements.size;
	xMemoryAllocteInfo.memoryTypeIndex = VkBuffer_CheckMemoryType(pxVkInstance, xMemoryRequirements.memoryTypeBits, xMemoryProperties);

	VK_CHECK(vkAllocateMemory(VkInstance_GetDevice(pxVkInstance), &xMemoryAllocteInfo, 0, &pxVkBuffer->xDeviceMemory));
	VK_CHECK(vkBindBufferMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xBuffer, pxVkBuffer->xDeviceMemory, 0));

	return pxVkBuffer;
}

void VkBuffer_Free(struct xVkBuffer_t* pxVkBuffer, struct xVkInstance_t* pxVkInstance) {
	vkDestroyBuffer(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xBuffer, 0);
	vkFreeMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xDeviceMemory, 0);
}

VkBuffer VkBuffer_GetBuffer(struct xVkBuffer_t* pxBuffer) {
	return pxBuffer->xBuffer;
}

VkDeviceMemory VkBuffer_GetDeviceMemory(struct xVkBuffer_t* pxBuffer) {
	return pxBuffer->xDeviceMemory;
}

void VkBuffer_Copy(struct xVkInstance_t* pxVkInstance, struct xVkBuffer_t* pxVkSourceBuffer, struct xVkBuffer_t* pxVkDestinationBuffer, uint64_t wSize) {
	VkCommandBufferAllocateInfo xCommandBufferAllocateInfo;
	memset(&xCommandBufferAllocateInfo, 0, sizeof(xCommandBufferAllocateInfo));
	xCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xCommandBufferAllocateInfo.commandPool = VkInstance_GetCommandPool(pxVkInstance);
	xCommandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer xCommandBuffer;
	VK_CHECK(vkAllocateCommandBuffers(VkInstance_GetDevice(pxVkInstance), &xCommandBufferAllocateInfo, &xCommandBuffer));

	VkCommandBufferBeginInfo xCommandBufferBeginInfo;
	memset(&xCommandBufferBeginInfo, 0, sizeof(xCommandBufferBeginInfo));
	xCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	xCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(xCommandBuffer, &xCommandBufferBeginInfo));

		VkBufferCopy xBufferCopy;
		memset(&xBufferCopy, 0, sizeof(xBufferCopy));
		xBufferCopy.size = wSize;
		vkCmdCopyBuffer(xCommandBuffer, pxVkSourceBuffer->xBuffer, pxVkDestinationBuffer->xBuffer, 1, &xBufferCopy);

	VK_CHECK(vkEndCommandBuffer(xCommandBuffer));

	VkSubmitInfo xSubmitInfo;
	memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
	xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	xSubmitInfo.commandBufferCount = 1;
	xSubmitInfo.pCommandBuffers = &xCommandBuffer;

	VK_CHECK(vkQueueSubmit(VkInstance_GetGraphicsQueue(pxVkInstance), 1, &xSubmitInfo, VK_NULL_HANDLE));
	VK_CHECK(vkQueueWaitIdle(VkInstance_GetGraphicsQueue(pxVkInstance)));

	vkFreeCommandBuffers(VkInstance_GetDevice(pxVkInstance), VkInstance_GetCommandPool(pxVkInstance), 1, &xCommandBuffer);
}
