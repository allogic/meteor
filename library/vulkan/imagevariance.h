#ifndef METEOR_VULKAN_IMAGEVARIANCE_H
#define METEOR_VULKAN_IMAGEVARIANCE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xImage_t;

struct xImage_t* StandardImage_Alloc(struct xInstance_t* pxInstance, char const* pcFilePath);
struct xImage_t* StorageImage_Alloc(struct xInstance_t* pxInstance, char const* pcFilePath);

#endif
