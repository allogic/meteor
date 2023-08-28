#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <macros.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkbuffer.h>
#include <vulkan/vkvertex.h>

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

struct xVkBuffer_t* VkVertexBuffer_Alloc(struct xVkInstance_t* pxVkInstance, struct xVkVertex_t* pxVertexBuffer, uint32_t nVertexCount) {
	struct xVkBuffer_t* pxVkBuffer = (struct xVkBuffer_t*)calloc(1, sizeof(struct xVkBuffer_t));

	VkBufferCreateInfo xBufferCreateInfo;
	memset(&xBufferCreateInfo, 0, sizeof(xBufferCreateInfo));
	xBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	xBufferCreateInfo.size = sizeof(struct xVkVertex_t) * nVertexCount;
	xBufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	xBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VK_CHECK(vkCreateBuffer(VkInstance_GetDevice(pxVkInstance), &xBufferCreateInfo, 0, &pxVkBuffer->xBuffer));

	VkMemoryRequirements xMemoryRequirements;
	vkGetBufferMemoryRequirements(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xBuffer, &xMemoryRequirements);

	VkMemoryAllocateInfo xMemoryAllocteInfo;
	memset(&xMemoryAllocteInfo, 0, sizeof(xMemoryAllocteInfo));
	xMemoryAllocteInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	xMemoryAllocteInfo.allocationSize = xMemoryRequirements.size;
	xMemoryAllocteInfo.memoryTypeIndex = VkBuffer_CheckMemoryType(pxVkInstance, xMemoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VK_CHECK(vkAllocateMemory(VkInstance_GetDevice(pxVkInstance), &xMemoryAllocteInfo, 0, &pxVkBuffer->xDeviceMemory));

	VK_CHECK(vkBindBufferMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xBuffer, pxVkBuffer->xDeviceMemory, 0));

	void* pMappedBuffer;
	VK_CHECK(vkMapMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xDeviceMemory, 0, xBufferCreateInfo.size, 0, &pMappedBuffer));
	memcpy(pMappedBuffer, pxVertexBuffer, xBufferCreateInfo.size);
	vkUnmapMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xDeviceMemory);

	return pxVkBuffer;
}

void VkVertexBuffer_Free(struct xVkBuffer_t* pxVkBuffer, struct xVkInstance_t* pxVkInstance) {
	vkDestroyBuffer(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xBuffer, 0);
	vkFreeMemory(VkInstance_GetDevice(pxVkInstance), pxVkBuffer->xDeviceMemory, 0);
}

VkBuffer VkBuffer_GetBuffer(struct xVkBuffer_t* pxBuffer) {
	return pxBuffer->xBuffer;
}

VkDeviceMemory VkBuffer_GetDeviceMemory(struct xVkBuffer_t* pxBuffer) {
	return pxBuffer->xDeviceMemory;
}
