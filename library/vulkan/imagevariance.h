#ifndef METEOR_IMAGEVARIANCE_H
#define METEOR_IMAGEVARIANCE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xImage_t;

struct xImage_t* VkTextureImage_Alloc(struct xInstance_t* pxInstance, char const* pcFilePath);

#endif
