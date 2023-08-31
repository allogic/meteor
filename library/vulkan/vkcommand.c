#include <stdio.h>
#include <string.h>

#include <common/macros.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkcommand.h>

VkCommandBuffer VkCommand_BeginSingleTimeCommands(struct xVkInstance_t* pxVkInstance) {
	VkCommandBufferAllocateInfo xCommandBufferAllocateInfo;
	memset(&xCommandBufferAllocateInfo, 0, sizeof(xCommandBufferAllocateInfo));
	xCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xCommandBufferAllocateInfo.commandPool = VkInstance_GetCommandPool(pxVkInstance);
	xCommandBufferAllocateInfo.commandBufferCount = 1;

	VkCommandBuffer xCommandBuffer;
	VK_CHECK(vkAllocateCommandBuffers(VkInstance_GetDevice(pxVkInstance), &xCommandBufferAllocateInfo, &xCommandBuffer));

	VkCommandBufferBeginInfo xCommandBufferBeginInfo;
	memset(&xCommandBufferBeginInfo, 0, sizeof(xCommandBufferBeginInfo));
	xCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	xCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VK_CHECK(vkBeginCommandBuffer(xCommandBuffer, &xCommandBufferBeginInfo));

	return xCommandBuffer;
}

void VkCommand_EndSingleTimeCommands(struct xVkInstance_t* pxVkInstance, VkCommandBuffer xCommandBuffer) {
	VK_CHECK(vkEndCommandBuffer(xCommandBuffer));

	VkSubmitInfo xSubmitInfo;
	memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
	xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	xSubmitInfo.commandBufferCount = 1;
	xSubmitInfo.pCommandBuffers = &xCommandBuffer;

	VK_CHECK(vkQueueSubmit(VkInstance_GetGraphicsQueue(pxVkInstance), 1, &xSubmitInfo, VK_NULL_HANDLE));
	VK_CHECK(vkQueueWaitIdle(VkInstance_GetGraphicsQueue(pxVkInstance)));

	vkFreeCommandBuffers(VkInstance_GetDevice(pxVkInstance), VkInstance_GetCommandPool(pxVkInstance), 1, &xCommandBuffer);
}
