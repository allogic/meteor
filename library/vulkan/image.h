#ifndef METEOR_IMAGE_H
#define METEOR_IMAGE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xImage_t;

struct xImage_t* VkImage_Alloc(struct xInstance_t* pxInstance, uint32_t nWidth, uint32_t nHeight, VkImageUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties, VkFormat xFormat, VkImageTiling xTiling);
void VkImage_Free(struct xImage_t* pxImage, struct xInstance_t* pxInstance);

VkImage VkImage_GetImage(struct xImage_t* pxImage);
VkDeviceMemory VkImage_GetDeviceMemory(struct xImage_t* pxImage);
void* VkImage_GetMappedData(struct xImage_t* pxImage);
void* VkImage_GetMappedDataRef(struct xImage_t* pxImage);

void VkImage_Map(struct xImage_t* pxImage, struct xInstance_t* pxInstance);
void VkImage_UnMap(struct xImage_t* pxImage, struct xInstance_t* pxInstance);

void VkImage_LayoutTransition(struct xImage_t* pxImage, struct xInstance_t* pxInstance, VkFormat xFormat, VkImageLayout xOldLayout, VkImageLayout xNewLayout);

//void VkBuffer_Copy(struct xInstance_t* pxInstance, struct xBuffer_t* pxSourceBuffer, struct xBuffer_t* pxDestinationBuffer, uint64_t wSize);
//void VkBuffer_CopyDirect(struct xImage_t* pxImage, void* pData, uint64_t wSize);

#endif
