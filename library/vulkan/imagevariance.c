#include <stdlib.h>
#include <string.h>

#include <filesystem/fileutil.h>

#include <vulkan/instance.h>
#include <vulkan/buffer.h>
#include <vulkan/image.h>
#include <vulkan/vertex.h>

struct xImage_t* VkTextureImage_Alloc(struct xInstance_t* pxInstance, char const* pcFilePath) {
	char* pData;
	uint64_t wSize;
	uint32_t nWidth;
	uint32_t nHeight;

	FileUtil_ReadBmp(&pData, &wSize, &nWidth, &nHeight, pcFilePath);

	struct xBuffer_t* pxStagingBuffer = VkBuffer_Alloc(pxInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkBuffer_Map(pxStagingBuffer, pxInstance);
	VkBuffer_Copy(pxStagingBuffer, pData, wSize);
	VkBuffer_UnMap(pxStagingBuffer, pxInstance);

	struct xImage_t* pxImage = VkImage_Alloc(pxInstance, nWidth, nHeight, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL);

	VkImage_LayoutTransition(pxImage, pxInstance, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	VkBuffer_CopyToImage(pxInstance, pxStagingBuffer, pxImage, nWidth, nHeight);
	VkImage_LayoutTransition(pxImage, pxInstance, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkBuffer_Free(pxStagingBuffer, pxInstance);

	free(pData);

	return pxImage;
}
