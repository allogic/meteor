#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkbuffer.h>
#include <vulkan/vkbuffervariants.h>

struct xVkBuffer_t* VkVertexBuffer_Alloc(struct xVkInstance_t* pxVkInstance, void* pData, uint64_t wSize) {
	struct xVkBuffer_t* pxVkStagingBuffer = VkBuffer_Alloc(pxVkInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* pMappedBuffer;
	vkMapMemory(VkInstance_GetDevice(pxVkInstance), VkBuffer_GetDeviceMemory(pxVkStagingBuffer), 0, wSize, 0, &pMappedBuffer);
	memcpy(pMappedBuffer, pData, wSize);
	vkUnmapMemory(VkInstance_GetDevice(pxVkInstance), VkBuffer_GetDeviceMemory(pxVkStagingBuffer));

	struct xVkBuffer_t* pxVkVertexBuffer = VkBuffer_Alloc(pxVkInstance, wSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkBuffer_Copy(pxVkInstance, pxVkStagingBuffer, pxVkVertexBuffer, wSize);
	VkBuffer_Free(pxVkStagingBuffer, pxVkInstance);

	return pxVkVertexBuffer;
}

struct xVkBuffer_t* VkIndexBuffer_Alloc(struct xVkInstance_t* pxVkInstance, void* pData, uint64_t wSize) {
	struct xVkBuffer_t* pxVkStagingBuffer = VkBuffer_Alloc(pxVkInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void* pMappedBuffer;
	vkMapMemory(VkInstance_GetDevice(pxVkInstance), VkBuffer_GetDeviceMemory(pxVkStagingBuffer), 0, wSize, 0, &pMappedBuffer);
	memcpy(pMappedBuffer, pData, wSize);
	vkUnmapMemory(VkInstance_GetDevice(pxVkInstance), VkBuffer_GetDeviceMemory(pxVkStagingBuffer));

	struct xVkBuffer_t* pxVkIndexBuffer = VkBuffer_Alloc(pxVkInstance, wSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkBuffer_Copy(pxVkInstance, pxVkStagingBuffer, pxVkIndexBuffer, wSize);
	VkBuffer_Free(pxVkStagingBuffer, pxVkInstance);

	return pxVkIndexBuffer;
}
