#ifndef METEOR_BUFFER_H
#define METEOR_BUFFER_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xVkInstance_t;
struct xVkBuffer_t;
struct xVkImage_t;

struct xVkBuffer_t* VkBuffer_Alloc(struct xVkInstance_t* pxVkInstance, uint64_t wSize, VkBufferUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties);
void VkBuffer_Free(struct xVkBuffer_t* pxVkBuffer, struct xVkInstance_t* pxVkInstance);

VkBuffer VkBuffer_GetBuffer(struct xVkBuffer_t* pxVkBuffer);
VkDeviceMemory VkBuffer_GetDeviceMemory(struct xVkBuffer_t* pxVkBuffer);
void* VkBuffer_GetMappedData(struct xVkBuffer_t* pxVkBuffer);
void* VkBuffer_GetMappedDataRef(struct xVkBuffer_t* pxVkBuffer);

void VkBuffer_Map(struct xVkBuffer_t* pxVkBuffer, struct xVkInstance_t* pxVkInstance);
void VkBuffer_UnMap(struct xVkBuffer_t* pxVkBuffer, struct xVkInstance_t* pxVkInstance);

void VkBuffer_CopyToBuffer(struct xVkInstance_t* pxVkInstance, struct xVkBuffer_t* pxVkSourceBuffer, struct xVkBuffer_t* pxVkDestinationBuffer, uint64_t wSize);
void VkBuffer_CopyToImage(struct xVkInstance_t* pxVkInstance, struct xVkBuffer_t* pxVkBuffer, struct xVkImage_t* pxVkImage, uint32_t nWidth, uint32_t nHeight);

void VkBuffer_Copy(struct xVkBuffer_t* pxVkBuffer, void* pData, uint64_t wSize);

#endif
