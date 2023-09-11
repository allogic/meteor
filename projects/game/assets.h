#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

#include <stdint.h>

struct xInstance_t;
struct xImage_t;

typedef struct {
	const char* pcName;
	const char* pcFilePath;
	struct xImage_t* pxImage;
} xImageAsset_t;

extern xImageAsset_t g_axImageAssets[];
extern const uint32_t g_nImageAssetsCount;

#define ASSET_IMAGE_ARRAY_BEGIN xImageAsset_t g_axImageAssets[] = {
#define ASSET_IMAGE_ARRAY_ENTRY(NAME, FILEPATH) { NAME, FILEPATH, 0 },
#define ASSET_IMAGE_ARRAY_END }; const uint32_t g_nImageAssetsCount = ARRAY_LENGTH(g_axImageAssets);

void Assets_Alloc(struct xInstance_t* pxInstance);
void Assets_Free(struct xInstance_t* pxInstance);

struct xImage_t* Assets_GetImageByIndex(uint32_t nIndex);
struct xImage_t* Assets_GetImageByName(const char* pcName);

#endif
