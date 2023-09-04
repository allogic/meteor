#include <stdlib.h>
#include <string.h>

#include <vulkan/instance.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>

struct xBuffer_t* VertexBuffer_Alloc(struct xInstance_t* pxInstance, void* pData, uint64_t wSize) {
	struct xBuffer_t* pxStagingBuffer = Buffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	Buffer_Map(pxStagingBuffer, pxInstance);
	Buffer_Copy(pxStagingBuffer, pData, wSize);
	Buffer_UnMap(pxStagingBuffer, pxInstance);

	struct xBuffer_t* pxVertexBuffer = Buffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	Buffer_CopyToBuffer(pxInstance, pxStagingBuffer, pxVertexBuffer, wSize);
	
	Buffer_Free(pxStagingBuffer, pxInstance);

	return pxVertexBuffer;
}

struct xBuffer_t* IndexBuffer_Alloc(struct xInstance_t* pxInstance, void* pData, uint64_t wSize) {
	struct xBuffer_t* pxStagingBuffer = Buffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	Buffer_Map(pxStagingBuffer, pxInstance);
	Buffer_Copy(pxStagingBuffer, pData, wSize);
	Buffer_UnMap(pxStagingBuffer, pxInstance);

	struct xBuffer_t* pxIndexBuffer = Buffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	Buffer_CopyToBuffer(pxInstance, pxStagingBuffer, pxIndexBuffer, wSize);

	Buffer_Free(pxStagingBuffer, pxInstance);

	return pxIndexBuffer;
}

struct xBuffer_t* UniformBuffer_Alloc(struct xInstance_t* pxInstance, uint64_t wSize) {
	struct xBuffer_t* pxUniformBuffer = Buffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	Buffer_Map(pxUniformBuffer, pxInstance);

	return pxUniformBuffer;
}

struct xBuffer_t* StorageBuffer_Alloc(struct xInstance_t* pxInstance, void* pData, uint64_t wSize) {
	struct xBuffer_t* pxStagingBuffer = Buffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	Buffer_Map(pxStagingBuffer, pxInstance);
	Buffer_Copy(pxStagingBuffer, pData, wSize);
	Buffer_UnMap(pxStagingBuffer, pxInstance);

	struct xBuffer_t* pxStorageBuffer = Buffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	Buffer_CopyToBuffer(pxInstance, pxStagingBuffer, pxStorageBuffer, wSize);

	Buffer_Free(pxStagingBuffer, pxInstance);

	return pxStorageBuffer;
}
