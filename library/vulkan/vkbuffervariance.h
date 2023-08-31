#ifndef METEOR_BUFFERVARIANCE_H
#define METEOR_BUFFERVARIANCE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xVkInstance_t;
struct xVkBuffer_t;

struct xVkBuffer_t* VkVertexBuffer_Alloc(struct xVkInstance_t* pxVkInstance, void* pData, uint64_t wSize);
struct xVkBuffer_t* VkIndexBuffer_Alloc(struct xVkInstance_t* pxVkInstance, void* pData, uint64_t wSize);
struct xVkBuffer_t* VkUniformBuffer_Alloc(struct xVkInstance_t* pxVkInstance, uint64_t wSize);

#endif
