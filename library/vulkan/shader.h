#ifndef METEOR_SHADER_H
#define METEOR_SHADER_H

#include <vulkan/vulkan.h>

struct xInstance_t;

void Shader_Alloc(struct xInstance_t* pxInstance, const char* pcVertFilePath, const char* pcFragFilePath, VkShaderModule* pxVertModule, VkShaderModule* pxFragModule);
void Shader_Free(struct xInstance_t* pxInstance, VkShaderModule xVertModule, VkShaderModule xFragModule);

#endif
