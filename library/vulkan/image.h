#ifndef METEOR_IMAGE_H
#define METEOR_IMAGE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xImage_t;

struct xImage_t* Image_Alloc(struct xInstance_t* pxInstance, uint32_t nWidth, uint32_t nHeight, VkImageUsageFlags xUsage, VkMemoryPropertyFlags xMemoryProperties, VkFormat xFormat, VkImageTiling xTiling, VkFilter xFilter);
void Image_Free(struct xImage_t* pxImage, struct xInstance_t* pxInstance);

uint64_t Image_GetSize(struct xImage_t* pxImage);
uint32_t Image_GetWidth(struct xImage_t* pxImage);
uint32_t Image_GetHeight(struct xImage_t* pxImage);
VkFormat Image_GetFormat(struct xImage_t* pxImage);
VkImage Image_GetImage(struct xImage_t* pxImage);
VkDeviceMemory Image_GetDeviceMemory(struct xImage_t* pxImage);
VkImageView Image_GetImageView(struct xImage_t* pxImage);
VkSampler Image_GetSampler(struct xImage_t* pxImage);
void* Image_GetMappedData(struct xImage_t* pxImage);
void* Image_GetMappedDataRef(struct xImage_t* pxImage);

void Image_Map(struct xImage_t* pxImage, struct xInstance_t* pxInstance);
void Image_UnMap(struct xImage_t* pxImage, struct xInstance_t* pxInstance);

void Image_LayoutTransition(struct xImage_t* pxImage, struct xInstance_t* pxInstance, VkFormat xFormat, VkImageLayout xOldLayout, VkImageLayout xNewLayout);

//void Buffer_Copy(struct xInstance_t* pxInstance, struct xBuffer_t* pxSourceBuffer, struct xBuffer_t* pxDestinationBuffer, uint64_t wSize);
//void Buffer_CopyDirect(struct xImage_t* pxImage, void* pData, uint64_t wSize);

#endif
