#ifndef METEOR_IMAGEVARIANCE_H
#define METEOR_IMAGEVARIANCE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xVkInstance_t;
struct xVkImage_t;

struct xVkImage_t* VkTextureImage_Alloc(struct xVkInstance_t* pxVkInstance, char const* pcFilePath);

#endif
