#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <platform/nativewindow.h>

#include <vulkan/instance.h>
#include <vulkan/swapchain.h>
#include <vulkan/shader.h>
#include <vulkan/graphicpipeline.h>
#include <vulkan/computepipeline.h>
#include <vulkan/vertex.h>
#include <vulkan/uniform.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>
#include <vulkan/image.h>

#include <sandsim/renderer.h>
#include <sandsim/storage.h>

struct xRenderer_t {
	VkDescriptorSetLayout xGraphicDescriptorSetLayout;
	VkDescriptorSetLayout xComputeDescriptorSetLayout;
	VkDescriptorPool xGraphicDescriptorPool;
	VkDescriptorPool xComputeDescriptorPool;
	VkDescriptorSet xGraphicDescriptorSet;
	VkDescriptorSet xComputeDescriptorSet;
	struct xBuffer_t* pxUniformTimeInfo;
	struct xBuffer_t* pxUniformModelViewProjection;
	struct xBuffer_t* pxStorageParticles;
	struct xGraphicPipeline_t* pxGraphicPipeline;
	struct xComputePipeline_t* pxComputePipeline;
	VkCommandBuffer xGraphicCommandBuffer;
	VkCommandBuffer xComputeCommandBuffer;
	VkSemaphore xImageAvailableSemaphore;
	VkSemaphore xGraphicFinishedSemaphore;
	VkSemaphore xComputeFinishedSemaphore;
	VkFence xGraphicInFlightFence;
	VkFence xComputeInFlightFence;
};

static void Renderer_CreateGraphicDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[2];

	axDescriptorSetLayoutBindings[0].binding = 0;
	axDescriptorSetLayoutBindings[0].descriptorCount = 1;
	axDescriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorSetLayoutBindings[0].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	axDescriptorSetLayoutBindings[1].binding = 1;
	axDescriptorSetLayoutBindings[1].descriptorCount = 1;
	axDescriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	axDescriptorSetLayoutBindings[1].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = ARRAY_LENGTH(axDescriptorSetLayoutBindings);
	xDescriptorSetLayoutCreateInfo.pBindings = axDescriptorSetLayoutBindings;
	//xDescriptorSetLayoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT; // TODO

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xGraphicDescriptorSetLayout));
}

static void Renderer_CreateComputeDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[3];

	axDescriptorSetLayoutBindings[0].binding = 0;
	axDescriptorSetLayoutBindings[0].descriptorCount = 1;
	axDescriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorSetLayoutBindings[0].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[1].binding = 1;
	axDescriptorSetLayoutBindings[1].descriptorCount = 1;
	axDescriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axDescriptorSetLayoutBindings[1].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[2].binding = 2;
	axDescriptorSetLayoutBindings[2].descriptorCount = 1;
	axDescriptorSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axDescriptorSetLayoutBindings[2].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = ARRAY_LENGTH(axDescriptorSetLayoutBindings);
	xDescriptorSetLayoutCreateInfo.pBindings = axDescriptorSetLayoutBindings;
	//xDescriptorSetLayoutCreateInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT; // TODO

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xComputeDescriptorSetLayout));
}

static void Renderer_CreateGraphicDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorPoolSize axDescriptorPoolSizes[2];

	axDescriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorPoolSizes[0].descriptorCount = 1;

	axDescriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	axDescriptorPoolSizes[1].descriptorCount = 1;

	VkDescriptorPoolCreateInfo xDescriptorPoolCreateInfo;
	memset(&xDescriptorPoolCreateInfo, 0, sizeof(xDescriptorPoolCreateInfo));
	xDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	xDescriptorPoolCreateInfo.poolSizeCount = ARRAY_LENGTH(axDescriptorPoolSizes);
	xDescriptorPoolCreateInfo.pPoolSizes = axDescriptorPoolSizes;
	xDescriptorPoolCreateInfo.maxSets = 1;
	//xDescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT; // TODO

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xGraphicDescriptorPool));
}

static void Renderer_CreateComputeDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorPoolSize axDescriptorPoolSizes[2];

	axDescriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorPoolSizes[0].descriptorCount = 1;

	axDescriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axDescriptorPoolSizes[1].descriptorCount = 2;

	VkDescriptorPoolCreateInfo xDescriptorPoolCreateInfo;
	memset(&xDescriptorPoolCreateInfo, 0, sizeof(xDescriptorPoolCreateInfo));
	xDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	xDescriptorPoolCreateInfo.poolSizeCount = ARRAY_LENGTH(axDescriptorPoolSizes);
	xDescriptorPoolCreateInfo.pPoolSizes = axDescriptorPoolSizes;
	xDescriptorPoolCreateInfo.maxSets = 1;
	//xDescriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT; // TODO

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xComputeDescriptorPool));
}

static void Renderer_CreateGraphicDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayout axDescriptorSetsLayouts[] = { pxRenderer->xGraphicDescriptorSetLayout };

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xGraphicDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = 1;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetsLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, &pxRenderer->xGraphicDescriptorSet));
}

static void Renderer_CreateComputeDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayout axDescriptorSetsLayouts[] = { pxRenderer->xComputeDescriptorSetLayout };

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xComputeDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = 1;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetsLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, &pxRenderer->xComputeDescriptorSet));
}

static void Renderer_UpdateGraphicDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xImage_t* pxImage) {
	VkDescriptorBufferInfo xModelViewProjectionDescriptorBufferInfo;
	memset(&xModelViewProjectionDescriptorBufferInfo, 0, sizeof(xModelViewProjectionDescriptorBufferInfo));
	xModelViewProjectionDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxUniformModelViewProjection);
	xModelViewProjectionDescriptorBufferInfo.offset = 0;
	xModelViewProjectionDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxUniformModelViewProjection);

	VkDescriptorImageInfo xImageDescriptorImageInfo;
	memset(&xImageDescriptorImageInfo, 0, sizeof(xImageDescriptorImageInfo));
	xImageDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	xImageDescriptorImageInfo.imageView = Image_GetImageView(pxImage);
	xImageDescriptorImageInfo.sampler = Image_GetSampler(pxImage);

	VkWriteDescriptorSet axWriteDescriptorSets[2];

	axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[0].pNext = 0;
	axWriteDescriptorSets[0].dstSet = pxRenderer->xGraphicDescriptorSet;
	axWriteDescriptorSets[0].dstBinding = 0;
	axWriteDescriptorSets[0].dstArrayElement = 0;
	axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[0].descriptorCount = 1;
	axWriteDescriptorSets[0].pImageInfo = 0;
	axWriteDescriptorSets[0].pBufferInfo = &xModelViewProjectionDescriptorBufferInfo;
	axWriteDescriptorSets[0].pTexelBufferView = 0;

	axWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[1].pNext = 0;
	axWriteDescriptorSets[1].dstSet = pxRenderer->xGraphicDescriptorSet;
	axWriteDescriptorSets[1].dstBinding = 1;
	axWriteDescriptorSets[1].dstArrayElement = 0;
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = &xImageDescriptorImageInfo;
	axWriteDescriptorSets[1].pBufferInfo = 0;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}

static void Renderer_UpdateComputeDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorBufferInfo xTimeInfoDescriptorBufferInfo;
	memset(&xTimeInfoDescriptorBufferInfo, 0, sizeof(xTimeInfoDescriptorBufferInfo));
	xTimeInfoDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxUniformTimeInfo);
	xTimeInfoDescriptorBufferInfo.offset = 0;
	xTimeInfoDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxUniformTimeInfo);

	VkDescriptorBufferInfo xParticleDescriptorBufferInfo;
	memset(&xParticleDescriptorBufferInfo, 0, sizeof(xParticleDescriptorBufferInfo));
	xParticleDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxStorageParticles);
	xParticleDescriptorBufferInfo.offset = 0;
	xParticleDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxStorageParticles);

	VkWriteDescriptorSet axWriteDescriptorSets[3];

	axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[0].pNext = 0;
	axWriteDescriptorSets[0].dstSet = pxRenderer->xComputeDescriptorSet;
	axWriteDescriptorSets[0].dstBinding = 0;
	axWriteDescriptorSets[0].dstArrayElement = 0;
	axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[0].descriptorCount = 1;
	axWriteDescriptorSets[0].pImageInfo = 0;
	axWriteDescriptorSets[0].pBufferInfo = &xTimeInfoDescriptorBufferInfo;
	axWriteDescriptorSets[0].pTexelBufferView = 0;

	axWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[1].pNext = 0;
	axWriteDescriptorSets[1].dstSet = pxRenderer->xComputeDescriptorSet;
	axWriteDescriptorSets[1].dstBinding = 1;
	axWriteDescriptorSets[1].dstArrayElement = 0;
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = 0;
	axWriteDescriptorSets[1].pBufferInfo = &xParticleDescriptorBufferInfo;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	axWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[2].pNext = 0;
	axWriteDescriptorSets[2].dstSet = pxRenderer->xComputeDescriptorSet;
	axWriteDescriptorSets[2].dstBinding = 2;
	axWriteDescriptorSets[2].dstArrayElement = 0;
	axWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axWriteDescriptorSets[2].descriptorCount = 1;
	axWriteDescriptorSets[2].pImageInfo = 0;
	axWriteDescriptorSets[2].pBufferInfo = &xParticleDescriptorBufferInfo;
	axWriteDescriptorSets[2].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}

static void Renderer_CreateGraphicPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	VkShaderModule xVertModule = Shader_Alloc(pxInstance, "shaders/default.vert.spv");
	VkShaderModule xFragModule = Shader_Alloc(pxInstance, "shaders/default.frag.spv");

	VkVertexInputBindingDescription xVertexInputBindingDescription;
	VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[3];

	Vertex_CreateDescription(&xVertexInputBindingDescription, axVertexInputAttributeDescriptions);

	pxRenderer->pxGraphicPipeline = GraphicPipeline_Alloc(pxInstance, pxSwapChain, xVertModule, xFragModule, xVertexInputBindingDescription, axVertexInputAttributeDescriptions, 3, pxRenderer->xGraphicDescriptorSetLayout);

	Shader_Free(pxInstance, xVertModule);
	Shader_Free(pxInstance, xFragModule);
}

static void Renderer_CreateComputePipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkShaderModule xCompModule = Shader_Alloc(pxInstance, "shaders/sand.comp.spv");

	pxRenderer->pxComputePipeline = ComputePipeline_Alloc(pxInstance, xCompModule, pxRenderer->xComputeDescriptorSetLayout);

	Shader_Free(pxInstance, xCompModule);
}

static void Renderer_CreateGraphicCommandBuffer(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkCommandBufferAllocateInfo xCommandBufferAllocCreateInfo;
	memset(&xCommandBufferAllocCreateInfo, 0, sizeof(xCommandBufferAllocCreateInfo));
	xCommandBufferAllocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xCommandBufferAllocCreateInfo.commandPool = Instance_GetCommandPool(pxInstance);
	xCommandBufferAllocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xCommandBufferAllocCreateInfo.commandBufferCount = 1;

	VK_CHECK(vkAllocateCommandBuffers(Instance_GetDevice(pxInstance), &xCommandBufferAllocCreateInfo, &pxRenderer->xGraphicCommandBuffer));
}

static void Renderer_CreateComputeCommandBuffer(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkCommandBufferAllocateInfo xCommandBufferAllocCreateInfo;
	memset(&xCommandBufferAllocCreateInfo, 0, sizeof(xCommandBufferAllocCreateInfo));
	xCommandBufferAllocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xCommandBufferAllocCreateInfo.commandPool = Instance_GetCommandPool(pxInstance);
	xCommandBufferAllocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xCommandBufferAllocCreateInfo.commandBufferCount = 1;

	VK_CHECK(vkAllocateCommandBuffers(Instance_GetDevice(pxInstance), &xCommandBufferAllocCreateInfo, &pxRenderer->xComputeCommandBuffer));
}

static void Renderer_RecordGraphicCommandBuffer(struct xRenderer_t* pxRenderer, struct xSwapChain_t* pxSwapChain, uint32_t nImageIndex, struct xBuffer_t* pxVertexBuffer, struct xBuffer_t* pxIndexBuffer, uint32_t nIndexCount) {
	VkCommandBufferBeginInfo xCommandBufferBeginInfo;
	memset(&xCommandBufferBeginInfo, 0, sizeof(xCommandBufferBeginInfo));
	xCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	xCommandBufferBeginInfo.flags = 0;
	xCommandBufferBeginInfo.pInheritanceInfo = 0;

	VK_CHECK(vkBeginCommandBuffer(pxRenderer->xGraphicCommandBuffer, &xCommandBufferBeginInfo));

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

	vkCmdBeginRenderPass(pxRenderer->xGraphicCommandBuffer, &xRenderPassCreateInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipeline(pxRenderer->pxGraphicPipeline));

		vkCmdSetViewport(pxRenderer->xGraphicCommandBuffer, 0, 1, &xViewport);
		vkCmdSetScissor(pxRenderer->xGraphicCommandBuffer, 0, 1, &xScissor);

		VkBuffer axVertexBuffers[] = { Buffer_GetBuffer(pxVertexBuffer) };
		uint64_t awOffsets[] = { 0 };

		vkCmdBindVertexBuffers(pxRenderer->xGraphicCommandBuffer, 0, 1, axVertexBuffers, awOffsets);
		vkCmdBindIndexBuffer(pxRenderer->xGraphicCommandBuffer, Buffer_GetBuffer(pxIndexBuffer), 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipelineLayout(pxRenderer->pxGraphicPipeline), 0, 1, &pxRenderer->xGraphicDescriptorSet, 0, 0);

		vkCmdDrawIndexed(pxRenderer->xGraphicCommandBuffer, nIndexCount, 1, 0, 0, 0);

	vkCmdEndRenderPass(pxRenderer->xGraphicCommandBuffer);

	VK_CHECK(vkEndCommandBuffer(pxRenderer->xGraphicCommandBuffer));
}

static void Renderer_RecordComputeCommandBuffer(struct xRenderer_t* pxRenderer) {
	VkCommandBufferBeginInfo xCommandBufferBeginInfo;
	memset(&xCommandBufferBeginInfo, 0, sizeof(xCommandBufferBeginInfo));
	xCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	xCommandBufferBeginInfo.flags = 0;
	xCommandBufferBeginInfo.pInheritanceInfo = 0;

	VK_CHECK(vkBeginCommandBuffer(pxRenderer->xComputeCommandBuffer, &xCommandBufferBeginInfo));

	vkCmdBindPipeline(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipeline(pxRenderer->pxComputePipeline));

	vkCmdBindDescriptorSets(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipelineLayout(pxRenderer->pxComputePipeline), 0, 1, &pxRenderer->xComputeDescriptorSet, 0, 0);

	vkCmdDispatch(pxRenderer->xComputeCommandBuffer, 256, 1, 1); // TODO

	VK_CHECK(vkEndCommandBuffer(pxRenderer->xComputeCommandBuffer));
}

static void Renderer_CreateGraphicSyncObjects(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkSemaphoreCreateInfo xSemaphoreCreateInfo;
	memset(&xSemaphoreCreateInfo, 0, sizeof(xSemaphoreCreateInfo));
	xSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo xFenceCreateInfo;
	memset(&xFenceCreateInfo, 0, sizeof(xFenceCreateInfo));
	xFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	xFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->xImageAvailableSemaphore));
	VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->xGraphicFinishedSemaphore));

	VK_CHECK(vkCreateFence(Instance_GetDevice(pxInstance), &xFenceCreateInfo, 0, &pxRenderer->xGraphicInFlightFence));
}

static void Renderer_CreateComputeSyncObjects(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkSemaphoreCreateInfo xSemaphoreCreateInfo;
	memset(&xSemaphoreCreateInfo, 0, sizeof(xSemaphoreCreateInfo));
	xSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo xFenceCreateInfo;
	memset(&xFenceCreateInfo, 0, sizeof(xFenceCreateInfo));
	xFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	xFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->xComputeFinishedSemaphore));

	VK_CHECK(vkCreateFence(Instance_GetDevice(pxInstance), &xFenceCreateInfo, 0, &pxRenderer->xComputeInFlightFence));
}

static void Renderer_SubmitCompute(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, xTimeInfo_t* pxTimeInfo) {
	VkSemaphore axSignalSemaphores[] = { pxRenderer->xComputeFinishedSemaphore };

	VkSubmitInfo xSubmitInfo;
	memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
	xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	xSubmitInfo.commandBufferCount = 1;
	xSubmitInfo.pCommandBuffers = &pxRenderer->xComputeCommandBuffer;
	xSubmitInfo.signalSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xSubmitInfo.pSignalSemaphores = axSignalSemaphores;

	VK_CHECK(vkWaitForFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xComputeInFlightFence, VK_TRUE, UINT64_MAX));

	Buffer_Copy(pxRenderer->pxUniformTimeInfo, pxTimeInfo, sizeof(xTimeInfo_t)); // TODO

	VK_CHECK(vkResetFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xComputeInFlightFence));

	VK_CHECK(vkResetCommandBuffer(pxRenderer->xComputeCommandBuffer, 0));

	Renderer_RecordComputeCommandBuffer(pxRenderer);

	VK_CHECK(vkQueueSubmit(Instance_GetComputeQueue(pxInstance), 1, &xSubmitInfo, pxRenderer->xComputeInFlightFence));
}

static void Renderer_SubmitGraphic(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xBuffer_t* pxVertexBuffer, struct xBuffer_t* pxIndexBuffer, uint32_t nIndexCount, struct xImage_t* pxImage) {
	VK_CHECK(vkWaitForFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xGraphicInFlightFence, VK_TRUE, UINT64_MAX));

	//Buffer_Copy(pxRenderer->pxUniformModelViewProjection, pxModelViewProjection, sizeof(xModelViewProjection_t)); // TODO

	uint32_t nImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(Instance_GetDevice(pxInstance), SwapChain_GetSwapChain(pxSwapChain), UINT64_MAX, pxRenderer->xImageAvailableSemaphore, VK_NULL_HANDLE, &nImageIndex));

	VK_CHECK(vkResetFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xGraphicInFlightFence));

	VK_CHECK(vkResetCommandBuffer(pxRenderer->xGraphicCommandBuffer, 0));

	Renderer_RecordGraphicCommandBuffer(pxRenderer, pxSwapChain, nImageIndex, pxVertexBuffer, pxIndexBuffer, nIndexCount);

	VkSemaphore axWaitSemaphores[] = { pxRenderer->xComputeFinishedSemaphore, pxRenderer->xImageAvailableSemaphore };
	VkSemaphore axSignalSemaphores[] = { pxRenderer->xGraphicFinishedSemaphore };
	VkPipelineStageFlags axWaitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSwapchainKHR axSwapChains[] = { SwapChain_GetSwapChain(pxSwapChain) };

	VkSubmitInfo xSubmitInfo;
	memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
	xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	xSubmitInfo.waitSemaphoreCount = ARRAY_LENGTH(axWaitSemaphores);
	xSubmitInfo.pWaitSemaphores = axWaitSemaphores;
	xSubmitInfo.pWaitDstStageMask = axWaitStages;
	xSubmitInfo.commandBufferCount = 1;
	xSubmitInfo.pCommandBuffers = &pxRenderer->xGraphicCommandBuffer;
	xSubmitInfo.signalSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xSubmitInfo.pSignalSemaphores = axSignalSemaphores;

	VK_CHECK(vkQueueSubmit(Instance_GetGraphicQueue(pxInstance), 1, &xSubmitInfo, pxRenderer->xGraphicInFlightFence));

	VkPresentInfoKHR xPresentInfo;
	memset(&xPresentInfo, 0, sizeof(xPresentInfo));
	xPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	xPresentInfo.waitSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xPresentInfo.pWaitSemaphores = axSignalSemaphores;
	xPresentInfo.swapchainCount = ARRAY_LENGTH(axSwapChains);
	xPresentInfo.pSwapchains = axSwapChains;
	xPresentInfo.pImageIndices = &nImageIndex;

	VK_CHECK(vkQueuePresentKHR(Instance_GetPresentQueue(pxInstance), &xPresentInfo));
}

struct xRenderer_t* Renderer_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xImage_t* pxImage) {
	struct xRenderer_t* pxRenderer = (struct xRenderer_t*)calloc(1, sizeof(struct xRenderer_t));

	pxRenderer->pxUniformTimeInfo = UniformBuffer_Alloc(pxInstance, sizeof(xTimeInfo_t));
	pxRenderer->pxUniformModelViewProjection = UniformBuffer_Alloc(pxInstance, sizeof(xModelViewProjection_t));

	xParticle_t axParticles[256]; // TODO

	pxRenderer->pxStorageParticles = StorageBuffer_Alloc(pxInstance, axParticles, sizeof(axParticles)); // TODO

	Renderer_CreateGraphicDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_CreateComputeDescriptorSetLayout(pxRenderer, pxInstance);	

	Renderer_CreateGraphicDescriptorPool(pxRenderer, pxInstance);
	Renderer_CreateComputeDescriptorPool(pxRenderer, pxInstance);

	Renderer_CreateGraphicDescriptorSets(pxRenderer, pxInstance);
	Renderer_CreateComputeDescriptorSets(pxRenderer, pxInstance);

	Renderer_UpdateGraphicDescriptorSets(pxRenderer, pxInstance, pxImage); // TODO
	Renderer_UpdateComputeDescriptorSets(pxRenderer, pxInstance);

	Renderer_CreateGraphicPipeline(pxRenderer, pxInstance, pxSwapChain);
	Renderer_CreateComputePipeline(pxRenderer, pxInstance);

	Renderer_CreateGraphicCommandBuffer(pxRenderer, pxInstance);
	Renderer_CreateComputeCommandBuffer(pxRenderer, pxInstance);

	Renderer_CreateGraphicSyncObjects(pxRenderer, pxInstance);
	Renderer_CreateComputeSyncObjects(pxRenderer, pxInstance);

	return pxRenderer;
}

void Renderer_Free(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyFence(Instance_GetDevice(pxInstance), pxRenderer->xComputeInFlightFence, 0);
	vkDestroyFence(Instance_GetDevice(pxInstance), pxRenderer->xGraphicInFlightFence, 0);

	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xComputeFinishedSemaphore, 0);
	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xGraphicFinishedSemaphore, 0);
	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xImageAvailableSemaphore, 0);

	ComputePipeline_Free(pxRenderer->pxComputePipeline, pxInstance);
	GraphicPipeline_Free(pxRenderer->pxGraphicPipeline, pxInstance);

	Buffer_Free(pxRenderer->pxStorageParticles, pxInstance);
	Buffer_Free(pxRenderer->pxUniformModelViewProjection, pxInstance);
	Buffer_Free(pxRenderer->pxUniformTimeInfo, pxInstance);

	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xComputeDescriptorPool, 0);
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xGraphicDescriptorPool, 0);

	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xComputeDescriptorSetLayout, 0);
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xGraphicDescriptorSetLayout, 0);

	free(pxRenderer);
}

void Renderer_UpdateTimeInfo(struct xRenderer_t* pxRenderer, xTimeInfo_t* pxTimeInfo) {
	Buffer_Copy(pxRenderer->pxUniformTimeInfo, pxTimeInfo, sizeof(xTimeInfo_t));
}

void Renderer_UpdateModelViewProjection(struct xRenderer_t* pxRenderer, xModelViewProjection_t* pxModelViewProjection) {
	Buffer_Copy(pxRenderer->pxUniformModelViewProjection, pxModelViewProjection, sizeof(xModelViewProjection_t));
}

void Renderer_Draw(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xBuffer_t* pxVertexBuffer, struct xBuffer_t* pxIndexBuffer, uint32_t nIndexCount, struct xImage_t* pxImage, xTimeInfo_t* pxTimeInfo) {
	Renderer_SubmitCompute(pxRenderer, pxInstance, pxTimeInfo);
	//Renderer_SubmitGraphic(pxRenderer, pxInstance, pxSwapChain, pxVertexBuffer, pxIndexBuffer, nIndexCount, pxImage);
}
