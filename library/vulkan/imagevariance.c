#include <stdlib.h>
#include <string.h>

#include <filesystem/fileutil.h>

#include <vulkan/instance.h>
#include <vulkan/buffer.h>
#include <vulkan/command.h>
#include <vulkan/image.h>
#include <vulkan/vertex.h>

struct xImage_t* StandardImage_Alloc(struct xInstance_t* pxInstance, char const* pcFilePath) {
	uint8_t* pData;
	uint64_t wSize;
	uint32_t nWidth;
	uint32_t nHeight;

	FileUtil_ReadBmp(&pData, &wSize, &nWidth, &nHeight, pcFilePath);

	struct xBuffer_t* pxStagingBuffer = Buffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	Buffer_Map(pxStagingBuffer, pxInstance);
	Buffer_SetTo(pxStagingBuffer, pData, wSize);
	Buffer_UnMap(pxStagingBuffer, pxInstance);

	struct xImage_t* pxImage = Image_Alloc(pxInstance, nWidth, nHeight, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_FILTER_NEAREST);

	VkCommandBuffer xCommandBuffer = Command_BeginSingle(pxInstance);

	Image_LayoutTransition(pxImage, xCommandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	Buffer_CopyToImage(pxStagingBuffer, xCommandBuffer, pxImage, nWidth, nHeight);
	Image_LayoutTransition(pxImage, xCommandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

	Command_EndSingle(pxInstance, xCommandBuffer);

	Buffer_Free(pxStagingBuffer, pxInstance);

	free(pData);

	return pxImage;
}

struct xImage_t* StorageImage_Alloc(struct xInstance_t* pxInstance, char const* pcFilePath) {
	uint8_t* pData;
	uint64_t wSize;
	uint32_t nWidth;
	uint32_t nHeight;

	FileUtil_ReadBmp(&pData, &wSize, &nWidth, &nHeight, pcFilePath);

	struct xBuffer_t* pxStagingBuffer = Buffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	Buffer_Map(pxStagingBuffer, pxInstance);
	Buffer_SetTo(pxStagingBuffer, pData, wSize);
	Buffer_UnMap(pxStagingBuffer, pxInstance);

	struct xImage_t* pxImage = Image_Alloc(pxInstance, nWidth, nHeight, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_FORMAT_B8G8R8A8_UNORM, VK_IMAGE_TILING_LINEAR, VK_FILTER_NEAREST);

	VkCommandBuffer xCommandBuffer = Command_BeginSingle(pxInstance);

	Image_LayoutTransition(pxImage, xCommandBuffer, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	Buffer_CopyToImage(pxStagingBuffer, xCommandBuffer, pxImage, nWidth, nHeight);
	Image_LayoutTransition(pxImage, xCommandBuffer, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_GENERAL);

	Command_EndSingle(pxInstance, xCommandBuffer);

	Buffer_Free(pxStagingBuffer, pxInstance);

	free(pData);

	return pxImage;
}
