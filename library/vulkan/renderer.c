#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <platform/nativewindow.h>

#include <vulkan/instance.h>
#include <vulkan/swapchain.h>
#include <vulkan/shader.h>
#include <vulkan/renderer.h>
#include <vulkan/uniform.h>
#include <vulkan/vertex.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>

#define MAX_FRAMES_IN_FLIGHT 2

struct xRenderer_t {
	VkDescriptorSetLayout xDescriptorSetLayout;
	VkDescriptorPool xDescriptorPool;
	VkDescriptorSet axDescriptorSet[MAX_FRAMES_IN_FLIGHT];
	struct xBuffer_t* apUniformBuffer[MAX_FRAMES_IN_FLIGHT];
	VkPipelineLayout xPipelineLayout;
	VkPipeline xGraphicsPipeline;
	VkCommandBuffer axCommandBuffer[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore axImageAvailableSemaphore[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore axRenderFinishedSemaphore[MAX_FRAMES_IN_FLIGHT];
	VkFence axInFlightFence[MAX_FRAMES_IN_FLIGHT];
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
	VkDescriptorSetLayout axDescriptorSetLayouts[MAX_FRAMES_IN_FLIGHT];

	for (int32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		axDescriptorSetLayouts[i] = pxRenderer->xDescriptorSetLayout;
	}

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = MAX_FRAMES_IN_FLIGHT;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, pxRenderer->axDescriptorSet));

	VkDescriptorBufferInfo xDescriptorBufferInfo;
	VkWriteDescriptorSet xWriteDescriptorSet;

	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		memset(&xDescriptorBufferInfo, 0, sizeof(xDescriptorBufferInfo));
		xDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->apUniformBuffer[i]);
		xDescriptorBufferInfo.offset = 0;
		xDescriptorBufferInfo.range = wSize;

		memset(&xWriteDescriptorSet, 0, sizeof(xWriteDescriptorSet));
		xWriteDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		xWriteDescriptorSet.dstSet = pxRenderer->axDescriptorSet[i];
		xWriteDescriptorSet.dstBinding = 0;
		xWriteDescriptorSet.dstArrayElement = 0;
		xWriteDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		xWriteDescriptorSet.descriptorCount = 1;
		xWriteDescriptorSet.pBufferInfo = &xDescriptorBufferInfo;

		vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), 1, &xWriteDescriptorSet, 0, 0);
	}
}

static void Renderer_CreateGraphicsPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, VkShaderModule xVertModule, VkShaderModule xFragModule) {
	VkPipelineShaderStageCreateInfo xVertShaderStageCreateInfo;
	memset(&xVertShaderStageCreateInfo, 0, sizeof(xVertShaderStageCreateInfo));
	xVertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	xVertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	xVertShaderStageCreateInfo.module = xVertModule;
	xVertShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo xFragShaderStageCreateInfo;
	memset(&xFragShaderStageCreateInfo, 0, sizeof(xFragShaderStageCreateInfo));
	xFragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	xFragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	xFragShaderStageCreateInfo.module = xFragModule;
	xFragShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo axShaderStages[] = { xVertShaderStageCreateInfo, xFragShaderStageCreateInfo };

	VkVertexInputBindingDescription xVertexInputBindingDescription;
	memset(&xVertexInputBindingDescription, 0, sizeof(xVertexInputBindingDescription));
	xVertexInputBindingDescription.binding = 0;
	xVertexInputBindingDescription.stride = sizeof(xVertex_t);
	xVertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[3];

	axVertexInputAttributeDescriptions[0].binding = 0;
	axVertexInputAttributeDescriptions[0].location = 0;
	axVertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	axVertexInputAttributeDescriptions[0].offset = 0;

	axVertexInputAttributeDescriptions[1].binding = 0;
	axVertexInputAttributeDescriptions[1].location = 1;
	axVertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	axVertexInputAttributeDescriptions[1].offset = sizeof(float) * 3;

	axVertexInputAttributeDescriptions[2].binding = 0;
	axVertexInputAttributeDescriptions[2].location = 2;
	axVertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	axVertexInputAttributeDescriptions[2].offset = sizeof(float) * 5;

	VkPipelineVertexInputStateCreateInfo xVertexInputCreateInfo;
	memset(&xVertexInputCreateInfo, 0, sizeof(xVertexInputCreateInfo));
	xVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	xVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	xVertexInputCreateInfo.pVertexBindingDescriptions = &xVertexInputBindingDescription;
	xVertexInputCreateInfo.vertexAttributeDescriptionCount = ARRAY_LENGTH(axVertexInputAttributeDescriptions);
	xVertexInputCreateInfo.pVertexAttributeDescriptions = axVertexInputAttributeDescriptions; 

	VkPipelineInputAssemblyStateCreateInfo xInputAssemblyCreateInfo;
	memset(&xInputAssemblyCreateInfo, 0, sizeof(xInputAssemblyCreateInfo));
	xInputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	xInputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	xInputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

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

	VkPipelineViewportStateCreateInfo xViewportStateCreateInfo;
	memset(&xViewportStateCreateInfo, 0, sizeof(xViewportStateCreateInfo));
	xViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	xViewportStateCreateInfo.viewportCount = 1;
	xViewportStateCreateInfo.pViewports = &xViewport;
	xViewportStateCreateInfo.scissorCount = 1;
	xViewportStateCreateInfo.pScissors = &xScissor;

	VkPipelineRasterizationStateCreateInfo xRasterizerCreateInfo;
	memset(&xRasterizerCreateInfo, 0, sizeof(xRasterizerCreateInfo));
	xRasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	xRasterizerCreateInfo.depthClampEnable = VK_FALSE;
	xRasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	xRasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	xRasterizerCreateInfo.lineWidth = 1.0F;
	xRasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	xRasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	xRasterizerCreateInfo.depthBiasEnable = VK_FALSE;
	xRasterizerCreateInfo.depthBiasConstantFactor = 0.0F;
	xRasterizerCreateInfo.depthBiasClamp = 0.0F;
	xRasterizerCreateInfo.depthBiasSlopeFactor = 0.0F;

	VkPipelineMultisampleStateCreateInfo xMultisamplingCreateInfo;
	memset(&xMultisamplingCreateInfo, 0, sizeof(xMultisamplingCreateInfo));
	xMultisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	xMultisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
	xMultisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	xMultisamplingCreateInfo.minSampleShading = 1.0F;
	xMultisamplingCreateInfo.pSampleMask = 0;
	xMultisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
	xMultisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState xColorBlendAttachment;
	memset(&xColorBlendAttachment, 0, sizeof(xColorBlendAttachment));
	xColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	xColorBlendAttachment.blendEnable = VK_FALSE;
	xColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	xColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	xColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	xColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	xColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	xColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo xColorBlendCreateInfo;
	memset(&xColorBlendCreateInfo, 0, sizeof(xColorBlendCreateInfo));
	xColorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	xColorBlendCreateInfo.logicOpEnable = VK_FALSE;
	xColorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	xColorBlendCreateInfo.attachmentCount = 1;
	xColorBlendCreateInfo.pAttachments = &xColorBlendAttachment;
	xColorBlendCreateInfo.blendConstants[0] = 0.0F;
	xColorBlendCreateInfo.blendConstants[1] = 0.0F;
	xColorBlendCreateInfo.blendConstants[2] = 0.0F;
	xColorBlendCreateInfo.blendConstants[3] = 0.0F;

	VkDynamicState axDynamicState[] = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

	VkPipelineDynamicStateCreateInfo xDynamicStateCreateInfo;
	memset(&xDynamicStateCreateInfo, 0, sizeof(xDynamicStateCreateInfo));
	xDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	xDynamicStateCreateInfo.dynamicStateCount = ARRAY_LENGTH(axDynamicState);
	xDynamicStateCreateInfo.pDynamicStates = axDynamicState;

	VkPipelineLayoutCreateInfo xPipelineLayoutCreateInfo;
	memset(&xPipelineLayoutCreateInfo, 0, sizeof(xPipelineLayoutCreateInfo));
	xPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	xPipelineLayoutCreateInfo.setLayoutCount = 1;
	xPipelineLayoutCreateInfo.pSetLayouts = &pxRenderer->xDescriptorSetLayout;
	xPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	xPipelineLayoutCreateInfo.pPushConstantRanges = 0;

	VK_CHECK(vkCreatePipelineLayout(Instance_GetDevice(pxInstance), &xPipelineLayoutCreateInfo, 0, &pxRenderer->xPipelineLayout));

	VkGraphicsPipelineCreateInfo xPipelineCreateInfo;
	memset(&xPipelineCreateInfo, 0, sizeof(xPipelineCreateInfo));
	xPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	xPipelineCreateInfo.stageCount = ARRAY_LENGTH(axShaderStages);
	xPipelineCreateInfo.pStages = axShaderStages;
	xPipelineCreateInfo.pVertexInputState = &xVertexInputCreateInfo;
	xPipelineCreateInfo.pInputAssemblyState = &xInputAssemblyCreateInfo;
	xPipelineCreateInfo.pViewportState = &xViewportStateCreateInfo;
	xPipelineCreateInfo.pRasterizationState = &xRasterizerCreateInfo;
	xPipelineCreateInfo.pMultisampleState = &xMultisamplingCreateInfo;
	xPipelineCreateInfo.pDepthStencilState = 0;
	xPipelineCreateInfo.pColorBlendState = &xColorBlendCreateInfo;
	xPipelineCreateInfo.pDynamicState = &xDynamicStateCreateInfo;
	xPipelineCreateInfo.layout = pxRenderer->xPipelineLayout;
	xPipelineCreateInfo.renderPass = SwapChain_GetRenderPass(pxSwapChain);
	xPipelineCreateInfo.subpass = 0;
	xPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	VK_CHECK(vkCreateGraphicsPipelines(Instance_GetDevice(pxInstance), VK_NULL_HANDLE, 1, &xPipelineCreateInfo, 0, &pxRenderer->xGraphicsPipeline));
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

	vkCmdBeginRenderPass(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], &xRenderPassCreateInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pxRenderer->xGraphicsPipeline);

		VkViewport xViewport;
		memset(&xViewport, 0, sizeof(xViewport));
		xViewport.x = 0.0F;
		xViewport.y = 0.0F;
		xViewport.width = (float)NativeWindow_GetWidth();
		xViewport.height = (float)NativeWindow_GetHeight();
		xViewport.minDepth = 0.0F;
		xViewport.maxDepth = 1.0F;
		vkCmdSetViewport(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], 0, 1, &xViewport);

		VkRect2D xScissor;
		memset(&xScissor, 0, sizeof(xScissor));
		xScissor.offset.x = 0;
		xScissor.offset.y = 0;
		xScissor.extent.width = NativeWindow_GetWidth();
		xScissor.extent.height = NativeWindow_GetHeight();
		vkCmdSetScissor(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], 0, 1, &xScissor);

		VkBuffer axVertexBuffers[] = { Buffer_GetBuffer(pxVertexBuffer) };
		uint64_t awOffsets[] = { 0 };

		vkCmdBindVertexBuffers(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], 0, 1, axVertexBuffers, awOffsets);
		vkCmdBindIndexBuffer(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], Buffer_GetBuffer(pxIndexBuffer), 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, pxRenderer->xPipelineLayout, 0, 1, &pxRenderer->axDescriptorSet[pxRenderer->nCurrentFrame], 0, 0);

		vkCmdDrawIndexed(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], nIndexCount, 1, 0, 0, 0);

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
		VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->axImageAvailableSemaphore[i]));
		VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->axRenderFinishedSemaphore[i]));

		VK_CHECK(vkCreateFence(Instance_GetDevice(pxInstance), &xFenceCreateInfo, 0, &pxRenderer->axInFlightFence[i]));
	}
}

struct xRenderer_t* Renderer_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	struct xRenderer_t* pxRenderer = (struct xRenderer_t*)calloc(1, sizeof(struct xRenderer_t));

	VkShaderModule xVertModule;
	VkShaderModule xFragModule;

	Shader_Alloc(pxInstance, "../shaders/test.vert.spv", "../shaders/test.frag.spv", &xVertModule, &xFragModule);

	Renderer_CreateUniformBuffer(pxRenderer, pxInstance, sizeof(xModelViewProjection_t));
	Renderer_CreateDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_CreateDescriptorPool(pxRenderer, pxInstance);
	Renderer_CreateDescriptorSets(pxRenderer, pxInstance, sizeof(xModelViewProjection_t));
	Renderer_CreateGraphicsPipeline(pxRenderer, pxInstance, pxSwapChain, xVertModule, xFragModule);
	Renderer_CreateCommandBuffers(pxRenderer, pxInstance);
	Renderer_CreatSyncObjects(pxRenderer, pxInstance);

	Shader_Free(pxInstance, xVertModule, xFragModule);

	return pxRenderer;
}

void Renderer_Free(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vkDestroyFence(Instance_GetDevice(pxInstance), pxRenderer->axInFlightFence[i], 0);

		vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->axRenderFinishedSemaphore[i], 0);
		vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->axImageAvailableSemaphore[i], 0);
	}

	vkDestroyPipeline(Instance_GetDevice(pxInstance), pxRenderer->xGraphicsPipeline, 0);

	vkDestroyPipelineLayout(Instance_GetDevice(pxInstance), pxRenderer->xPipelineLayout, 0);

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
	VK_CHECK(vkWaitForFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->axInFlightFence[pxRenderer->nCurrentFrame], VK_TRUE, UINT64_MAX));

	uint32_t nImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(Instance_GetDevice(pxInstance), SwapChain_GetSwapChain(pxSwapChain), UINT64_MAX, pxRenderer->axImageAvailableSemaphore[pxRenderer->nCurrentFrame], VK_NULL_HANDLE, &nImageIndex));

	VK_CHECK(vkResetFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->axInFlightFence[pxRenderer->nCurrentFrame]));

	VK_CHECK(vkResetCommandBuffer(pxRenderer->axCommandBuffer[pxRenderer->nCurrentFrame], 0));

	Renderer_RecordCommandBuffer(pxRenderer, pxSwapChain, nImageIndex, pxVertexBuffer, pxIndexBuffer, nIndexCount);

	VkSemaphore axWaitSemaphores[] = { pxRenderer->axImageAvailableSemaphore[pxRenderer->nCurrentFrame] };
	VkSemaphore axSignalSemaphores[] = { pxRenderer->axRenderFinishedSemaphore[pxRenderer->nCurrentFrame] };
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

	VK_CHECK(vkQueueSubmit(Instance_GetGraphicsQueue(pxInstance), 1, &xSubmitInfo, pxRenderer->axInFlightFence[pxRenderer->nCurrentFrame]));

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
