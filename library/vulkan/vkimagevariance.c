#include <stdlib.h>
#include <string.h>

#include <filesystem/fileutil.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkbuffer.h>
#include <vulkan/vkimage.h>
#include <vulkan/vkvertex.h>

struct xVkImage_t* VkTextureImage_Alloc(struct xVkInstance_t* pxVkInstance, char const* pcFilePath) {
	char* pData;
	uint64_t wSize;
	uint32_t nWidth;
	uint32_t nHeight;

	FileUtil_ReadBmp(&pData, &wSize, &nWidth, &nHeight, pcFilePath);

	struct xVkBuffer_t* pxVkStagingBuffer = VkBuffer_Alloc(pxVkInstance, wSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkBuffer_Map(pxVkStagingBuffer, pxVkInstance);
	VkBuffer_Copy(pxVkStagingBuffer, pData, wSize);
	VkBuffer_UnMap(pxVkStagingBuffer, pxVkInstance);

	struct xVkImage_t* pxVkImage = VkImage_Alloc(pxVkInstance, nWidth, nHeight, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL);

	VkImage_LayoutTransition(pxVkImage, pxVkInstance, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	VkBuffer_CopyToImage(pxVkInstance, pxVkStagingBuffer, pxVkImage, nWidth, nHeight);
	VkImage_LayoutTransition(pxVkImage, pxVkInstance, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkBuffer_Free(pxVkStagingBuffer, pxVkInstance);

	free(pData);

	return pxVkImage;
}
