#ifndef METEOR_VULKAN_SHADER_H
#define METEOR_VULKAN_SHADER_H

#include <vulkan/vulkan.h>

struct xInstance_t;

VkShaderModule Shader_Alloc(struct xInstance_t* pxInstance, const char* pcFilePath);
void Shader_Free(struct xInstance_t* pxInstance, VkShaderModule xShaderModule);

#endif
