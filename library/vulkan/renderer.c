#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <platform/nativewindow.h>

#include <vulkan/instance.h>
#include <vulkan/swapchain.h>
#include <vulkan/shader.h>
#include <vulkan/renderer.h>
#include <vulkan/graphicpipeline.h>
#include <vulkan/vertex.h>
#include <vulkan/uniform.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>

#define MAX_FRAMES_IN_FLIGHT 2

struct xRenderer_t {
	VkDescriptorSetLayout xDescriptorSetLayout;
	VkDescriptorPool xDescriptorPool;
	VkDescriptorSet axDescriptorSets[MAX_FRAMES_IN_FLIGHT];
	struct xBuffer_t* apUniformBuffer[MAX_FRAMES_IN_FLIGHT];
	struct xGraphicPipeline_t* pxDefaultPipeline;
	struct xGraphicPipeline_t* pxDebugPipeline;
	struct xGraphicPipeline_t* pxInterfacePipeline;
	VkCommandBuffer axCommandBuffer[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore axImageAvailableSemaphores[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore axRenderFinishedSemaphores[MAX_FRAMES_IN_FLIGHT];
	VkFence axInFlightFences[MAX_FRAMES_IN_FLIGHT];
	uint32_t nCurrentFrame;
};

static void Renderer_CreateUniformBuffer(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint64_t wSize) {
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		pxRenderer->apUniformBuffer[i] = UniformBuffer_Alloc(pxInstance, wSize);
	}
}

static void Renderer_CreateDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayoutBinding xDescriptorSetLayoutBinding;
	memset(&xDescriptorSetLayoutBinding, 0, sizeof(xDescriptorSetLayoutBinding));
	xDescriptorSetLayoutBinding.binding = 0;
	xDescriptorSetLayoutBinding.descriptorCount = 1;
	xDescriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	xDescriptorSetLayoutBinding.pImmutableSamplers = 0;
	xDescriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = 1;
	xDescriptorSetLayoutCreateInfo.pBindings = &xDescriptorSetLayoutBinding;

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xDescriptorSetLayout));
}

static void Renderer_CreateDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorPoolSize xDescriptorPoolSize;
	memset(&xDescriptorPoolSize, 0, sizeof(xDescriptorPoolSize));
	xDescriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	xDescriptorPoolSize.descriptorCount = MAX_FRAMES_IN_FLIGHT;

	VkDescriptorPoolCreateInfo xDescriptorPoolCreateInfo;
	memset(&xDescriptorPoolCreateInfo, 0, sizeof(xDescriptorPoolCreateInfo));
	xDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	xDescriptorPoolCreateInfo.poolSizeCount = 1;
	xDescriptorPoolCreateInfo.pPoolSizes = &xDescriptorPoolSize;
	xDescriptorPoolCreateInfo.maxSets = MAX_FRAMES_IN_FLIGHT;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xDescriptorPool));
}

static void Renderer_CreateDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint64_t wSize) {
	VkDescriptorSetLayout axDescriptorSetsLayouts[MAX_FRAMES_IN_FLIGHT];

	for (int32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		axDescriptorSetsLayouts[i] = pxRenderer->xDescriptorSetLayout;
	}

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetsLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, pxRenderer->axDescriptorSets));

	VkDescriptorBufferInfo xDescriptorBufferInfo;
	VkWriteDescriptorSet xWriteDescriptorSet;

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		memset(&xDescriptorBufferInfo, 0, sizeof(xDescriptorBufferInfo));
		xDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->apUniformBuffer[i]);
		xDescriptorBufferInfo.offset = 0;
		xDescriptorBufferInfo.range = wSize;

		memset(&xWriteDescriptorSet, 0, sizeof(xWriteDescriptorSet));
		xWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		xWriteDescriptorSet.dstSet = pxRenderer->axDescriptorSets[i];
		xWriteDescriptorSet.dstBinding = 0;
		xWriteDescriptorSet.dstArrayElement = 0;
		xWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		xWriteDescriptorSet.descriptorCount = 1;
		xWriteDescriptorSet.pBufferInfo = &xDescriptorBufferInfo;

		vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), 1, &xWriteDescriptorSet, 0, 0);
	}
}

static void Renderer_CreateGraphicPipelines(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	VkShaderModule xDefaultVertModule, xDefaultFragModule;
	VkShaderModule xDebugVertModule, xDebugFragModule;
	VkShaderModule xInterfaceVertModule, xInterfaceFragModule;

	Shader_Alloc(pxInstance, "shaders/default.vert.spv", "shaders/default.frag.spv", &xDefaultVertModule, &xDefaultFragModule);
	Shader_Alloc(pxInstance, "shaders/debug.vert.spv", "shaders/debug.frag.spv", &xDebugVertModule, &xDebugFragModule);
	Shader_Alloc(pxInstance, "shaders/interface.vert.spv", "shaders/interface.frag.spv", &xInterfaceVertModule, &xInterfaceFragModule);

	VkVertexInputBindingDescription xDefaultVertexInputBindingDescription;
	VkVertexInputBindingDescription xDebugVertexInputBindingDescription;
	VkVertexInputBindingDescription xInterfaceVertexInputBindingDescription;

	VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[3];
	VkVertexInputAttributeDescription axDebugInputAttributeDescriptions[2];
	VkVertexInputAttributeDescription axInterfaceInputAttributeDescriptions[3];

	Vertex_DefaultDescription(&xDefaultVertexInputBindingDescription, axVertexInputAttributeDescriptions);
	Vertex_DebugDescription(&xDebugVertexInputBindingDescription, axDebugInputAttributeDescriptions);
	Vertex_InterfaceDescription(&xInterfaceVertexInputBindingDescription, axInterfaceInputAttributeDescriptions);

	pxRenderer->pxDefaultPipeline = GraphicPipeline_Alloc(pxInstance, pxSwapChain, xDefaultVertModule, xDefaultFragModule, xDefaultVertexInputBindingDescription, axVertexInputAttributeDescriptions, 3, pxRenderer->xDescriptorSetLayout);
	pxRenderer->pxDebugPipeline = GraphicPipeline_Alloc(pxInstance, pxSwapChain, xDebugVertModule, xDebugFragModule, xDebugVertexInputBindingDescription, axDebugInputAttributeDescriptions, 2, pxRenderer->xDescriptorSetLayout);
	pxRenderer->pxInterfacePipeline = GraphicPipeline_Alloc(pxInstance, pxSwapChain, xInterfaceVertModule, xInterfaceFragModule, xInterfaceVertexInputBindingDescription, axInterfaceInputAttributeDescriptions, 3, pxRenderer->xDescriptorSetLayout);

	Shader_Free(pxInstance, xInterfaceVertModule, xInterfaceFragModule);
	Shader_Free(pxInstance, xDebugVertModule, xDebugFragModule);
	Shader_Free(pxInstance, xDefaultVertModule, xDefaultFragModule);
}

static void Renderer_CreateCommandBuffers(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkCommandBufferAllocateInfo xCommandBufferAllocCreateInfo;
	memset(&xCommandBufferAllocCreateInfo, 0, sizeof(xCommandBufferAllocCreateInfo));
	xCommandBufferAllocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xCommandBufferAllocCreateInfo.commandPool = Instance_GetCommandPool(pxInstance);
	xCommandBufferAllocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xCommandBufferAllocCreateInfo.commandBufferCount = MAX_FRAMES_IN_FLIGHT;

	VK_CHECK(vkAllocateCommandBuffers(Instance_GetDevice(pxInstance), &xCommandBufferAllocCreateInfo, pxRenderer->axCommandBuffer));
}

static void Renderer_RecordCommandBuffer(struct xRenderer_t* pxRenderer, struct xSwapChain_t* pxSwapChain, uint32_t nImageIndex, struct xBuffer_t* pxVertexBuffer, struct xBuffer_t* pxIndexBuffer, uint32_t nIndexCount) {
	VkCommandBufferBeginInfo xCommandBufferBeginInfo;
	memset(&xCommandBufferBeginInfo, 0, sizeof(xCommandBufferBeginInfo));
	xCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	xCommandBufferBeginInfo.flags = 0;
	xCommandBufferBeginInfo.pInheritanceInfo = 0;

	VK_CHECK(vkBeginCommandBuffer(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], &xCommandBufferBeginInfo));

	VkClearValue xClearColor;
	memset(&xClearColor, 0, sizeof(xClearColor));
	xClearColor.color.float32[0] = 0.0F;
	xClearColor.color.float32[1] = 0.0F;
	xClearColor.color.float32[2] = 0.0F;
	xClearColor.color.float32[3] = 1.0F;

	VkRenderPassBeginInfo xRenderPassCreateInfo;
	memset(&xRenderPassCreateInfo, 0, sizeof(xRenderPassCreateInfo));
	xRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	xRenderPassCreateInfo.renderPass = SwapChain_GetRenderPass(pxSwapChain);
	xRenderPassCreateInfo.framebuffer = SwapChain_GetFrameBuffer(pxSwapChain, nImageIndex);
	xRenderPassCreateInfo.renderArea.offset.x = 0;
	xRenderPassCreateInfo.renderArea.offset.y = 0;
	xRenderPassCreateInfo.renderArea.extent.width = NativeWindow_GetWidth();
	xRenderPassCreateInfo.renderArea.extent.height = NativeWindow_GetHeight();
	xRenderPassCreateInfo.clearValueCount = 1;
	xRenderPassCreateInfo.pClearValues = &xClearColor;

	VkViewport xViewport;
	memset(&xViewport, 0, sizeof(xViewport));
	xViewport.x = 0.0F;
	xViewport.y = 0.0F;
	xViewport.width = (float)NativeWindow_GetWidth();
	xViewport.height = (float)NativeWindow_GetHeight();
	xViewport.minDepth = 0.0F;
	xViewport.maxDepth = 1.0F;

	VkRect2D xScissor;
	memset(&xScissor, 0, sizeof(xScissor));
	xScissor.offset.x = 0;
	xScissor.offset.y = 0;
	xScissor.extent.width = NativeWindow_GetWidth();
	xScissor.extent.height = NativeWindow_GetHeight();

	vkCmdBeginRenderPass(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], &xRenderPassCreateInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipeline(pxRenderer->pxDefaultPipeline));

		vkCmdSetViewport(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], 0, 1, &xViewport);
		vkCmdSetScissor(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], 0, 1, &xScissor);

		VkBuffer axVertexBuffers[] = { Buffer_GetBuffer(pxVertexBuffer) };
		uint64_t awOffsets[] = { 0 };

		vkCmdBindVertexBuffers(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], 0, 1, axVertexBuffers, awOffsets);
		vkCmdBindIndexBuffer(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], Buffer_GetBuffer(pxIndexBuffer), 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipelineLayout(pxRenderer->pxDefaultPipeline), 0, 1, &pxRenderer->axDescriptorSets[pxRenderer->nCurrentFrame], 0, 0);

		vkCmdDrawIndexed(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], nIndexCount, 1, 0, 0, 0);

		// TODO: Draw debug

		// TODO: Draw interface

	vkCmdEndRenderPass(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame]);

	VK_CHECK(vkEndCommandBuffer(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame]));
}

static void Renderer_CreatSyncObjects(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkSemaphoreCreateInfo xSemaphoreCreateInfo;
	memset(&xSemaphoreCreateInfo, 0, sizeof(xSemaphoreCreateInfo));
	xSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo xFenceCreateInfo;
	memset(&xFenceCreateInfo, 0, sizeof(xFenceCreateInfo));
	xFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	xFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->axImageAvailableSemaphores[i]));
		VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->axRenderFinishedSemaphores[i]));

		VK_CHECK(vkCreateFence(Instance_GetDevice(pxInstance), &xFenceCreateInfo, 0, &pxRenderer->axInFlightFences[i]));
	}
}

struct xRenderer_t* Renderer_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	struct xRenderer_t* pxRenderer = (struct xRenderer_t*)calloc(1, sizeof(struct xRenderer_t));

	Renderer_CreateUniformBuffer(pxRenderer, pxInstance, sizeof(xModelViewProjection_t));
	Renderer_CreateDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_CreateDescriptorPool(pxRenderer, pxInstance);
	Renderer_CreateDescriptorSets(pxRenderer, pxInstance, sizeof(xModelViewProjection_t));
	Renderer_CreateGraphicPipelines(pxRenderer, pxInstance, pxSwapChain);
	Renderer_CreateCommandBuffers(pxRenderer, pxInstance);
	Renderer_CreatSyncObjects(pxRenderer, pxInstance);

	return pxRenderer;
}

void Renderer_Free(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vkDestroyFence(Instance_GetDevice(pxInstance), pxRenderer->axInFlightFences[i], 0);

		vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->axRenderFinishedSemaphores[i], 0);
		vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->axImageAvailableSemaphores[i], 0);
	}

	GraphicPipeline_Free(pxRenderer->pxInterfacePipeline, pxInstance);
	GraphicPipeline_Free(pxRenderer->pxDebugPipeline, pxInstance);
	GraphicPipeline_Free(pxRenderer->pxDefaultPipeline, pxInstance);

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		Buffer_Free(pxRenderer->apUniformBuffer[i], pxInstance);
	}

	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xDescriptorPool, 0);

	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xDescriptorSetLayout, 0);

	free(pxRenderer);
}

void Renderer_UpdateModelViewProjection(struct xRenderer_t* pxRenderer, void* pData) {
	Buffer_Copy(pxRenderer->apUniformBuffer[pxRenderer->nCurrentFrame], pData, sizeof(xModelViewProjection_t));
}

void Renderer_Draw(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xBuffer_t* pxVertexBuffer, struct xBuffer_t* pxIndexBuffer, uint32_t nIndexCount) {
	VK_CHECK(vkWaitForFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->axInFlightFences[pxRenderer->nCurrentFrame], VK_TRUE, UINT64_MAX));

	uint32_t nImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(Instance_GetDevice(pxInstance), SwapChain_GetSwapChain(pxSwapChain), UINT64_MAX, pxRenderer->axImageAvailableSemaphores[pxRenderer->nCurrentFrame], VK_NULL_HANDLE, &nImageIndex));

	VK_CHECK(vkResetFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->axInFlightFences[pxRenderer->nCurrentFrame]));

	VK_CHECK(vkResetCommandBuffer(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], 0));

	Renderer_RecordCommandBuffer(pxRenderer, pxSwapChain, nImageIndex, pxVertexBuffer, pxIndexBuffer, nIndexCount);

	VkSemaphore axWaitSemaphores[] = { pxRenderer->axImageAvailableSemaphores[pxRenderer->nCurrentFrame] };
	VkSemaphore axSignalSemaphores[] = { pxRenderer->axRenderFinishedSemaphores[pxRenderer->nCurrentFrame] };
	VkPipelineStageFlags axWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSwapchainKHR axSwapChains[] = { SwapChain_GetSwapChain(pxSwapChain) };

	VkSubmitInfo xSubmitInfo;
	memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
	xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	xSubmitInfo.waitSemaphoreCount = ARRAY_LENGTH(axWaitSemaphores);
	xSubmitInfo.pWaitSemaphores = axWaitSemaphores;
	xSubmitInfo.pWaitDstStageMask = axWaitStages;
	xSubmitInfo.commandBufferCount = 1;
	xSubmitInfo.pCommandBuffers = &pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame];
	xSubmitInfo.signalSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xSubmitInfo.pSignalSemaphores = axSignalSemaphores;

	VK_CHECK(vkQueueSubmit(Instance_GetGraphicsQueue(pxInstance), 1, &xSubmitInfo, pxRenderer->axInFlightFences[pxRenderer->nCurrentFrame]));

	VkPresentInfoKHR xPresentInfo;
	memset(&xPresentInfo, 0, sizeof(xPresentInfo));
	xPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	xPresentInfo.waitSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xPresentInfo.pWaitSemaphores = axSignalSemaphores;
	xPresentInfo.swapchainCount = ARRAY_LENGTH(axSwapChains);
	xPresentInfo.pSwapchains = axSwapChains;
	xPresentInfo.pImageIndices = &nImageIndex;

	VK_CHECK(vkQueuePresentKHR(Instance_GetPresentQueue(pxInstance), &xPresentInfo));

	pxRenderer->nCurrentFrame = (pxRenderer->nCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
