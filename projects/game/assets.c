#include <string.h>

#include <common/macros.h>

#include <standard/list.h>

#include <vulkan/instance.h>
#include <vulkan/image.h>
#include <vulkan/imagevariance.h>

#include <game/assets.h>

#define ASSET_NAME_LENGTH 32

struct xImageAsset_t {
	char acName[ASSET_NAME_LENGTH];
	struct xImage_t* pxImage;
};

static struct xList_t* s_xImageList;

void Assets_Alloc(void) {
	s_xImageList = List_Alloc();
}

void Assets_Free(struct xInstance_t* pxInstance) {
	struct xImageAsset_t* pxImageAsset = List_Begin(s_xImageList);
	while (pxImageAsset) {
		Image_Free(pxImageAsset->pxImage, pxInstance);

		pxImageAsset = List_Next(s_xImageList);
	}

	List_Free(s_xImageList);
}

void Assets_CreateImageForName(struct xInstance_t* pxInstance, const char* pcName, const char* pcFilePath) {
	struct xImageAsset_t xImageAsset;
	memset(&xImageAsset, 0, sizeof(xImageAsset));

	uint32_t nNameLength = strlen(pcName);
	memcpy(xImageAsset.acName, pcName, MIN(ASSET_NAME_LENGTH - 1, nNameLength));
	xImageAsset.acName[ASSET_NAME_LENGTH - 1] = 0;

	xImageAsset.pxImage = StandardImage_Alloc(pxInstance, pcFilePath);

	List_Push(s_xImageList, &xImageAsset, sizeof(xImageAsset));
}

struct xImage_t* Assets_GetImageByName(const char* pcName) {
	struct xImageAsset_t* pxImageAsset = List_Begin(s_xImageList);
	while (pxImageAsset) {
		if (strcmp(pxImageAsset->acName, pcName) == 0) {
			return pxImageAsset->pxImage;
		}

		pxImageAsset = List_Next(s_xImageList);
	}

	return 0;
}
