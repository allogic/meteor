#ifndef METEOR_VULKAN_BUFFER_H
#define METEOR_VULKAN_BUFFER_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xBuffer_t;
struct xImage_t;

struct xBuffer_t* Buffer_Alloc(struct xInstance_t* pxInstance, uint64_t wSize, VkBufferUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties);
void Buffer_Free(struct xBuffer_t* pxBuffer, struct xInstance_t* pxInstance);

uint64_t Buffer_GetSize(struct xBuffer_t* pxBuffer);
VkBuffer Buffer_GetBuffer(struct xBuffer_t* pxBuffer);
VkDeviceMemory Buffer_GetDeviceMemory(struct xBuffer_t* pxBuffer);
void* Buffer_GetMappedData(struct xBuffer_t* pxBuffer);
void* Buffer_GetMappedDataRef(struct xBuffer_t* pxBuffer);

void Buffer_Map(struct xBuffer_t* pxBuffer, struct xInstance_t* pxInstance);
void Buffer_UnMap(struct xBuffer_t* pxBuffer, struct xInstance_t* pxInstance);

void Buffer_CopyToBuffer(struct xBuffer_t* pxBuffer, VkCommandBuffer xCommandBuffer, struct xBuffer_t* pxTarget, uint64_t wSize);
void Buffer_CopyToImage(struct xBuffer_t* pxBuffer, VkCommandBuffer xCommandBuffer, struct xImage_t* pxTarget, uint32_t nWidth, uint32_t nHeight);

void Buffer_SetTo(struct xBuffer_t* pxBuffer, void* pData, uint64_t wSize);

#endif
