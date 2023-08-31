#ifndef METEOR_IMAGE_H
#define METEOR_IMAGE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xVkInstance_t;
struct xVkImage_t;

struct xVkImage_t* VkImage_Alloc(struct xVkInstance_t* pxVkInstance, uint32_t nWidth, uint32_t nHeight, VkImageUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties, VkFormat xFormat, VkImageTiling xTiling);
void VkImage_Free(struct xVkImage_t* pxVkImage, struct xVkInstance_t* pxVkInstance);

VkImage VkImage_GetImage(struct xVkImage_t* pxVkImage);
VkDeviceMemory VkImage_GetDeviceMemory(struct xVkImage_t* pxVkImage);
void* VkImage_GetMappedData(struct xVkImage_t* pxVkImage);
void* VkImage_GetMappedDataRef(struct xVkImage_t* pxVkImage);

void VkImage_Map(struct xVkImage_t* pxVkImage, struct xVkInstance_t* pxVkInstance);
void VkImage_UnMap(struct xVkImage_t* pxVkImage, struct xVkInstance_t* pxVkInstance);

void VkImage_LayoutTransition(struct xVkImage_t* pxVkImage, struct xVkInstance_t* pxVkInstance, VkFormat xFormat, VkImageLayout xOldLayout, VkImageLayout xNewLayout);

//void VkBuffer_Copy(struct xVkInstance_t* pxVkInstance, struct xVkBuffer_t* pxVkSourceBuffer, struct xVkBuffer_t* pxVkDestinationBuffer, uint64_t wSize);
//void VkBuffer_CopyDirect(struct xVkImage_t* pxVkImage, void* pData, uint64_t wSize);

#endif
