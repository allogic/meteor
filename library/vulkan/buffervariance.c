#include <stdlib.h>
#include <string.h>

#include <vulkan/instance.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>

struct xBuffer_t* VkVertexBuffer_Alloc(struct xInstance_t* pxInstance, void* pData, uint64_t wSize) {
	struct xBuffer_t* pxStagingBuffer = VkBuffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkBuffer_Map(pxStagingBuffer, pxInstance);
	VkBuffer_Copy(pxStagingBuffer, pData, wSize);
	VkBuffer_UnMap(pxStagingBuffer, pxInstance);

	struct xBuffer_t* pxVertexBuffer = VkBuffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkBuffer_CopyToBuffer(pxInstance, pxStagingBuffer, pxVertexBuffer, wSize);
	
	VkBuffer_Free(pxStagingBuffer, pxInstance);

	return pxVertexBuffer;
}

struct xBuffer_t* VkIndexBuffer_Alloc(struct xInstance_t* pxInstance, void* pData, uint64_t wSize) {
	struct xBuffer_t* pxStagingBuffer = VkBuffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkBuffer_Map(pxStagingBuffer, pxInstance);
	VkBuffer_Copy(pxStagingBuffer, pData, wSize);
	VkBuffer_UnMap(pxStagingBuffer, pxInstance);

	struct xBuffer_t* pxIndexBuffer = VkBuffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkBuffer_CopyToBuffer(pxInstance, pxStagingBuffer, pxIndexBuffer, wSize);

	VkBuffer_Free(pxStagingBuffer, pxInstance);

	return pxIndexBuffer;
}

struct xBuffer_t* VkUniformBuffer_Alloc(struct xInstance_t* pxInstance, uint64_t wSize) {
	struct xBuffer_t* pxUniformBuffer = VkBuffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkBuffer_Map(pxUniformBuffer, pxInstance);

	return pxUniformBuffer;
}
