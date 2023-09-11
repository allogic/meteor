#include <string.h>

#include <common/macros.h>

#include <vulkan/instance.h>
#include <vulkan/image.h>
#include <vulkan/imagevariance.h>

#include <game/assets.h>

void Assets_Alloc(struct xInstance_t* pxInstance) {
	for (uint32_t i = 0; i < g_nImageAssetsCount; ++i) {
		g_axImageAssets[i].pxImage = StandardImage_Alloc(pxInstance, g_axImageAssets[i].pcFilePath);
	}
}

void Assets_Free(struct xInstance_t* pxInstance) {
	for (uint32_t i = 0; i < g_nImageAssetsCount; ++i) {
		Image_Free(g_axImageAssets[i].pxImage, pxInstance);
	}
}

struct xImage_t* Assets_GetImageByIndex(uint32_t nIndex) {
	return g_axImageAssets[nIndex].pxImage;
}

struct xImage_t* Assets_GetImageByName(const char* pcName) {
	for (uint32_t i = 0; i < g_nImageAssetsCount; ++i) {
		if (strcmp(g_axImageAssets[i].pcName, pcName) == 0) {
			return g_axImageAssets[i].pxImage;
		}
	}

	return 0;
}
