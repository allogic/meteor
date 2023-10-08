#include <stdio.h>
#include <string.h>

#include <common/macros.h>

#include <vulkan/common.h>
#include <vulkan/instance.h>
#include <vulkan/command.h>

VkCommandBuffer Command_BeginSingle(struct xInstance_t* pxInstance) {
	VkCommandBufferAllocateInfo xCommandBufferAllocateInfo;
	memset(&xCommandBufferAllocateInfo, 0, sizeof(xCommandBufferAllocateInfo));
	xCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xCommandBufferAllocateInfo.commandPool = Instance_GetCommandPool(pxInstance);
	xCommandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer xCommandBuffer;
	VK_CHECK(vkAllocateCommandBuffers(Instance_GetDevice(pxInstance), &xCommandBufferAllocateInfo, &xCommandBuffer));

	VkCommandBufferBeginInfo xCommandBufferBeginInfo;
	memset(&xCommandBufferBeginInfo, 0, sizeof(xCommandBufferBeginInfo));
	xCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	xCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(xCommandBuffer, &xCommandBufferBeginInfo));

	return xCommandBuffer;
}

void Command_EndSingle(struct xInstance_t* pxInstance, VkCommandBuffer xCommandBuffer) {
	VK_CHECK(vkEndCommandBuffer(xCommandBuffer));

	VkSubmitInfo xSubmitInfo;
	memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
	xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	xSubmitInfo.commandBufferCount = 1;
	xSubmitInfo.pCommandBuffers = &xCommandBuffer;

	VK_CHECK(vkQueueSubmit(Instance_GetGraphicQueue(pxInstance), 1, &xSubmitInfo, 0));
	VK_CHECK(vkQueueWaitIdle(Instance_GetGraphicQueue(pxInstance)));

	vkFreeCommandBuffers(Instance_GetDevice(pxInstance), Instance_GetCommandPool(pxInstance), 1, &xCommandBuffer);
}
