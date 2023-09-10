#ifndef GAME_RENDERER_H
#define GAME_RENDERER_H

#include <stdint.h>

#include <vulkan/uniform.h>
#include <vulkan/vulkan.h>

struct xInstance_t;
struct xSwapChain_t;
struct xRenderer_t;
struct xList_t;

struct xRenderer_t* Renderer_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain);
void Renderer_Free(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance);

xTimeInfo_t* Renderer_GetTimeInfo(struct xRenderer_t* pxRenderer);
xViewProjection_t* Renderer_GetViewProjection(struct xRenderer_t* pxRenderer);

void Renderer_Draw(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xList_t* pxEntities);

#endif
