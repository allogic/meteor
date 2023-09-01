#ifndef METEOR_BUFFER_H
#define METEOR_BUFFER_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xBuffer_t;
struct xImage_t;

struct xBuffer_t* VkBuffer_Alloc(struct xInstance_t* pxInstance, uint64_t wSize, VkBufferUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties);
void VkBuffer_Free(struct xBuffer_t* pxBuffer, struct xInstance_t* pxInstance);

VkBuffer VkBuffer_GetBuffer(struct xBuffer_t* pxBuffer);
VkDeviceMemory VkBuffer_GetDeviceMemory(struct xBuffer_t* pxBuffer);
void* VkBuffer_GetMappedData(struct xBuffer_t* pxBuffer);
void* VkBuffer_GetMappedDataRef(struct xBuffer_t* pxBuffer);

void VkBuffer_Map(struct xBuffer_t* pxBuffer, struct xInstance_t* pxInstance);
void VkBuffer_UnMap(struct xBuffer_t* pxBuffer, struct xInstance_t* pxInstance);

void VkBuffer_CopyToBuffer(struct xInstance_t* pxInstance, struct xBuffer_t* pxSourceBuffer, struct xBuffer_t* pxDestinationBuffer, uint64_t wSize);
void VkBuffer_CopyToImage(struct xInstance_t* pxInstance, struct xBuffer_t* pxBuffer, struct xImage_t* pxImage, uint32_t nWidth, uint32_t nHeight);

void VkBuffer_Copy(struct xBuffer_t* pxBuffer, void* pData, uint64_t wSize);

#endif
