#ifndef METEOR_SHADER_H
#define METEOR_SHADER_H

#include <vulkan/vulkan.h>

struct xVkInstance_t;

void VkShader_Alloc(struct xVkInstance_t* pxVkInstance, const char* pcVertFilePath, const char* pcFragFilePath, VkShaderModule* pxVertModule, VkShaderModule* pxFragModule);
void VkShader_Free(struct xVkInstance_t* pxVkInstance, VkShaderModule xVertModule, VkShaderModule xFragModule);

#endif
