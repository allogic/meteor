#ifndef METEOR_COMMAND_H
#define METEOR_COMMAND_H

#include <vulkan/vulkan.h>

struct xVkInstance_t;

VkCommandBuffer VkCommand_BeginSingleTimeCommands(struct xVkInstance_t* pxVkInstance);
void VkCommand_EndSingleTimeCommands(struct xVkInstance_t* pxVkInstance, VkCommandBuffer xCommandBuffer);

#endif
