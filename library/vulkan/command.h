#ifndef METEOR_COMMAND_H
#define METEOR_COMMAND_H

#include <vulkan/vulkan.h>

struct xInstance_t;

VkCommandBuffer VkCommand_BeginSingleTimeCommands(struct xInstance_t* pxInstance);
void VkCommand_EndSingleTimeCommands(struct xInstance_t* pxInstance, VkCommandBuffer xCommandBuffer);

#endif
