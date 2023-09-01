#ifndef METEOR_BUFFERVARIANCE_H
#define METEOR_BUFFERVARIANCE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xBuffer_t;

struct xBuffer_t* VkVertexBuffer_Alloc(struct xInstance_t* pxInstance, void* pData, uint64_t wSize);
struct xBuffer_t* VkIndexBuffer_Alloc(struct xInstance_t* pxInstance, void* pData, uint64_t wSize);
struct xBuffer_t* VkUniformBuffer_Alloc(struct xInstance_t* pxInstance, uint64_t wSize);

#endif