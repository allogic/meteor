#ifndef METEOR_VULKAN_BUFFERVARIANCE_H
#define METEOR_VULKAN_BUFFERVARIANCE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xBuffer_t;

struct xBuffer_t* VertexBuffer_AllocDevice(struct xInstance_t* pxInstance, void* pData, uint64_t wSize);
struct xBuffer_t* VertexBuffer_AllocCoherent(struct xInstance_t* pxInstance, uint64_t wSize);

struct xBuffer_t* IndexBuffer_AllocDevice(struct xInstance_t* pxInstance, void* pData, uint64_t wSize);
struct xBuffer_t* IndexBuffer_AllocCoherent(struct xInstance_t* pxInstance, uint64_t wSize);

struct xBuffer_t* UniformBuffer_AllocDevice(struct xInstance_t* pxInstance, void* pData, uint64_t wSize);
struct xBuffer_t* UniformBuffer_AllocCoherent(struct xInstance_t* pxInstance, uint64_t wSize);

struct xBuffer_t* StorageBuffer_AllocDevice(struct xInstance_t* pxInstance, void* pData, uint64_t wSize);
struct xBuffer_t* StorageBuffer_AllocCoherent(struct xInstance_t* pxInstance, uint64_t wSize);

#endif
