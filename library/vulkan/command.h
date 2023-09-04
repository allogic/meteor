#ifndef METEOR_COMMAND_H
#define METEOR_COMMAND_H

#include <vulkan/vulkan.h>

struct xInstance_t;

VkCommandBuffer Command_BeginSingle(struct xInstance_t* pxInstance);
void Command_EndSingle(struct xInstance_t* pxInstance, VkCommandBuffer xCommandBuffer);

#endif
