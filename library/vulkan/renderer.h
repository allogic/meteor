#ifndef METEOR_RENDERER_H
#define METEOR_RENDERER_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xSwapChain_t;
struct xRenderer_t;
struct xBuffer_t;

struct xRenderer_t* VkRenderer_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain);
void VkRenderer_Free(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance);

void VkRenderer_UpdateModelViewProjection(struct xRenderer_t* pxRenderer, void* pData);
void VkRenderer_Draw(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xBuffer_t* pxVertexBuffer, struct xBuffer_t* pxIndexBuffer, uint32_t nIndexCount);

#endif