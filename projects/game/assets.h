#ifndef GAME_ASSETS_H
#define GAME_ASSETS_H

struct xInstance_t;
struct xImage_t;

void Assets_Alloc(void);
void Assets_Free(struct xInstance_t* pxInstance);

void Assets_CreateImageForName(struct xInstance_t* pxInstance, const char* pcName, const char* pcFilePath);

struct xImage_t* Assets_GetImageByName(const char* pcName);

#endif
