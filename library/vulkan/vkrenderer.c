#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <macros.h>

#include <platform/nativewindow.h>

#include <vulkan/vkinstance.h>
#include <vulkan/vkswapchain.h>
#include <vulkan/vkshader.h>
#include <vulkan/vkrenderer.h>

struct xVkRenderer_t {
	VkRenderPass xRenderPass;
	VkPipelineLayout xPipelineLayout;
	VkPipeline xGraphicsPipeline;
	VkFramebuffer* pxFrameBuffers;
	VkCommandPool xCommandPool;
	VkCommandBuffer xCommandBuffer;
	VkSemaphore xImageAvailableSemaphore;
	VkSemaphore xRenderFinishedSemaphore;
	VkFence xInFlightFence;
};

static void VkRenderer_CreateRenderPass(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance) {
	VkAttachmentDescription xColorAttachmentDesc;
	memset(&xColorAttachmentDesc, 0, sizeof(xColorAttachmentDesc));
	xColorAttachmentDesc.format = VkInstance_GetPreferedSurfaceFormat(pxVkInstance);
	xColorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
	xColorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	xColorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	xColorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	xColorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	xColorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	xColorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference xColorAttachmentRef;
	memset(&xColorAttachmentRef, 0, sizeof(xColorAttachmentRef));
	xColorAttachmentRef.attachment = 0;
	xColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription xSubpassDesc;
	memset(&xSubpassDesc, 0, sizeof(xSubpassDesc));
	xSubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	xSubpassDesc.colorAttachmentCount = 1;
	xSubpassDesc.pColorAttachments = &xColorAttachmentRef;

	VkSubpassDependency xSubpassDependency;
	memset(&xSubpassDependency, 0, sizeof(xSubpassDependency));
	xSubpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	xSubpassDependency.dstSubpass = 0;
	xSubpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	xSubpassDependency.srcAccessMask = 0;
	xSubpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	xSubpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	VkRenderPassCreateInfo xRenderPassCreateInfo;
	memset(&xRenderPassCreateInfo, 0, sizeof(xRenderPassCreateInfo));
	xRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	xRenderPassCreateInfo.attachmentCount = 1;
	xRenderPassCreateInfo.pAttachments = &xColorAttachmentDesc;
	xRenderPassCreateInfo.subpassCount = 1;
	xRenderPassCreateInfo.pSubpasses = &xSubpassDesc;
	xRenderPassCreateInfo.dependencyCount = 1;
	xRenderPassCreateInfo.pDependencies = &xSubpassDependency;

	VK_CHECK(vkCreateRenderPass(VkInstance_GetDevice(pxVkInstance), &xRenderPassCreateInfo, 0, &pxVkRenderer->xRenderPass));
}

static void VkRenderer_CreateGraphicsPipeline(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance, VkShaderModule xVertModule, VkShaderModule xFragModule) {
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

	VkPipelineVertexInputStateCreateInfo xVertexInputCreateInfo;
	memset(&xVertexInputCreateInfo, 0, sizeof(xVertexInputCreateInfo));
	xVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	xVertexInputCreateInfo.vertexBindingDescriptionCount = 0;
	xVertexInputCreateInfo.vertexAttributeDescriptionCount = 0;

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
	xPipelineLayoutCreateInfo.setLayoutCount = 0;
	xPipelineLayoutCreateInfo.pSetLayouts = 0;
	xPipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	xPipelineLayoutCreateInfo.pPushConstantRanges = 0;

	VK_CHECK(vkCreatePipelineLayout(VkInstance_GetDevice(pxVkInstance), &xPipelineLayoutCreateInfo, 0, &pxVkRenderer->xPipelineLayout));

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
	xPipelineCreateInfo.layout = pxVkRenderer->xPipelineLayout;
	xPipelineCreateInfo.renderPass = pxVkRenderer->xRenderPass;
	xPipelineCreateInfo.subpass = 0;
	xPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	VK_CHECK(vkCreateGraphicsPipelines(VkInstance_GetDevice(pxVkInstance), VK_NULL_HANDLE, 1, &xPipelineCreateInfo, 0, &pxVkRenderer->xGraphicsPipeline));
}

static void VkRenderer_CreateFrameBuffers(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance, struct xVkSwapChain_t* pxVkSwapChain) {
	uint32_t nSwapChainImageCount = VkSwapChain_GetImageCount(pxVkSwapChain);

	pxVkRenderer->pxFrameBuffers = (VkFramebuffer*)malloc(sizeof(VkFramebuffer) * nSwapChainImageCount);

	for (uint32_t i = 0; i < nSwapChainImageCount; ++i) {
    	VkImageView axAttachments[] = {
        	VkSwapChain_GetImageView(pxVkSwapChain, i),
    	};

    	VkFramebufferCreateInfo xFramebufferCreateInfo;
		memset(&xFramebufferCreateInfo, 0, sizeof(xFramebufferCreateInfo));
    	xFramebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    	xFramebufferCreateInfo.renderPass = pxVkRenderer->xRenderPass;
    	xFramebufferCreateInfo.attachmentCount = ARRAY_LENGTH(axAttachments);
    	xFramebufferCreateInfo.pAttachments = axAttachments;
    	xFramebufferCreateInfo.width = NativeWindow_GetWidth();
    	xFramebufferCreateInfo.height = NativeWindow_GetHeight();
    	xFramebufferCreateInfo.layers = 1;

    	VK_CHECK(vkCreateFramebuffer(VkInstance_GetDevice(pxVkInstance), &xFramebufferCreateInfo, 0, &pxVkRenderer->pxFrameBuffers[i]));
	}
}

static void VkRenderer_CreateCommandPool(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance) {
	VkCommandPoolCreateInfo xCommandPoolCreateInfo;
	memset(&xCommandPoolCreateInfo, 0, sizeof(xCommandPoolCreateInfo));
	xCommandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	xCommandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	xCommandPoolCreateInfo.queueFamilyIndex = VkInstance_GetGraphicsQueueIndex(pxVkInstance);

	VK_CHECK(vkCreateCommandPool(VkInstance_GetDevice(pxVkInstance), &xCommandPoolCreateInfo, 0, &pxVkRenderer->xCommandPool));
}

static void VkRenderer_CreateCommandBuffer(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance) {
	VkCommandBufferAllocateInfo xCommandBufferAllocCreateInfo;
	memset(&xCommandBufferAllocCreateInfo, 0, sizeof(xCommandBufferAllocCreateInfo));
	xCommandBufferAllocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xCommandBufferAllocCreateInfo.commandPool = pxVkRenderer->xCommandPool;
	xCommandBufferAllocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xCommandBufferAllocCreateInfo.commandBufferCount = 1;

	VK_CHECK(vkAllocateCommandBuffers(VkInstance_GetDevice(pxVkInstance), &xCommandBufferAllocCreateInfo, &pxVkRenderer->xCommandBuffer));
}

static void VkRenderer_RecordCommandBuffer(struct xVkRenderer_t* pxVkRenderer, uint32_t nImageIndex) {
	VkCommandBufferBeginInfo xCommandBufferBeginInfo;
	memset(&xCommandBufferBeginInfo, 0, sizeof(xCommandBufferBeginInfo));
	xCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	xCommandBufferBeginInfo.flags = 0;
	xCommandBufferBeginInfo.pInheritanceInfo = 0;

	VK_CHECK(vkBeginCommandBuffer(pxVkRenderer->xCommandBuffer, &xCommandBufferBeginInfo));

	VkClearValue xClearColor;
	memset(&xClearColor, 0, sizeof(xClearColor));
	xClearColor.color.float32[0] = 0.0F;
	xClearColor.color.float32[1] = 0.0F;
	xClearColor.color.float32[2] = 0.0F;
	xClearColor.color.float32[3] = 1.0F;

	VkRenderPassBeginInfo xRenderPassCreateInfo;
	memset(&xRenderPassCreateInfo, 0, sizeof(xRenderPassCreateInfo));
	xRenderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	xRenderPassCreateInfo.renderPass = pxVkRenderer->xRenderPass;
	xRenderPassCreateInfo.framebuffer = pxVkRenderer->pxFrameBuffers[nImageIndex];
	xRenderPassCreateInfo.renderArea.offset.x = 0;
	xRenderPassCreateInfo.renderArea.offset.y = 0;
	xRenderPassCreateInfo.renderArea.extent.width = NativeWindow_GetWidth();
	xRenderPassCreateInfo.renderArea.extent.height = NativeWindow_GetHeight();
	xRenderPassCreateInfo.clearValueCount = 1;
	xRenderPassCreateInfo.pClearValues = &xClearColor;

	vkCmdBeginRenderPass(pxVkRenderer->xCommandBuffer, &xRenderPassCreateInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(pxVkRenderer->xCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pxVkRenderer->xGraphicsPipeline);

		VkViewport xViewport;
		memset(&xViewport, 0, sizeof(xViewport));
		xViewport.x = 0.0F;
		xViewport.y = 0.0F;
		xViewport.width = (float)NativeWindow_GetWidth();
		xViewport.height = (float)NativeWindow_GetHeight();
		xViewport.minDepth = 0.0F;
		xViewport.maxDepth = 1.0F;
		vkCmdSetViewport(pxVkRenderer->xCommandBuffer, 0, 1, &xViewport);

		VkRect2D xScissor;
		memset(&xScissor, 0, sizeof(xScissor));
		xScissor.offset.x = 0;
		xScissor.offset.y = 0;
		xScissor.extent.width = NativeWindow_GetWidth();
		xScissor.extent.height = NativeWindow_GetHeight();
		vkCmdSetScissor(pxVkRenderer->xCommandBuffer, 0, 1, &xScissor);

		vkCmdDraw(pxVkRenderer->xCommandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(pxVkRenderer->xCommandBuffer);

	VK_CHECK(vkEndCommandBuffer(pxVkRenderer->xCommandBuffer));
}

static void VkRenderer_CreatSyncObjects(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance) {
	VkSemaphoreCreateInfo xSemaphoreCreateInfo;
	memset(&xSemaphoreCreateInfo, 0, sizeof(xSemaphoreCreateInfo));
    xSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo xFenceCreateInfo;
	memset(&xFenceCreateInfo, 0, sizeof(xFenceCreateInfo));
	xFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	xFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateSemaphore(VkInstance_GetDevice(pxVkInstance), &xSemaphoreCreateInfo, 0, &pxVkRenderer->xImageAvailableSemaphore));
    VK_CHECK(vkCreateSemaphore(VkInstance_GetDevice(pxVkInstance), &xSemaphoreCreateInfo, 0, &pxVkRenderer->xRenderFinishedSemaphore));

    VK_CHECK(vkCreateFence(VkInstance_GetDevice(pxVkInstance), &xFenceCreateInfo, 0, &pxVkRenderer->xInFlightFence));
}

struct xVkRenderer_t* VkRenderer_Alloc(struct xVkInstance_t* pxVkInstance, struct xVkSwapChain_t* pxVkSwapChain) {
	struct xVkRenderer_t* pxVkRenderer = (struct xVkRenderer_t*)calloc(1, sizeof(struct xVkRenderer_t));

	VkShaderModule xVertModule;
	VkShaderModule xFragModule;

	VkShader_Alloc(pxVkInstance, "../shaders/test.vert.spv", "../shaders/test.frag.spv", &xVertModule, &xFragModule);

	VkRenderer_CreateRenderPass(pxVkRenderer, pxVkInstance);
	VkRenderer_CreateGraphicsPipeline(pxVkRenderer, pxVkInstance, xVertModule, xFragModule);
	VkRenderer_CreateFrameBuffers(pxVkRenderer, pxVkInstance, pxVkSwapChain);
	VkRenderer_CreateCommandPool(pxVkRenderer, pxVkInstance);
	VkRenderer_CreateCommandBuffer(pxVkRenderer, pxVkInstance);
	VkRenderer_CreatSyncObjects(pxVkRenderer, pxVkInstance);

	VkShader_Free(pxVkInstance, xVertModule, xFragModule);

	return pxVkRenderer;
}

void VkRenderer_Free(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance, struct xVkSwapChain_t* pxVkSwapChain) {
	VK_CHECK(vkDeviceWaitIdle(VkInstance_GetDevice(pxVkInstance)));

	vkDestroyFence(VkInstance_GetDevice(pxVkInstance), pxVkRenderer->xInFlightFence, 0);

	vkDestroySemaphore(VkInstance_GetDevice(pxVkInstance), pxVkRenderer->xRenderFinishedSemaphore, 0);
	vkDestroySemaphore(VkInstance_GetDevice(pxVkInstance), pxVkRenderer->xImageAvailableSemaphore, 0);

	vkDestroyCommandPool(VkInstance_GetDevice(pxVkInstance), pxVkRenderer->xCommandPool, 0);

	for (uint32_t i = 0; i < VkSwapChain_GetImageCount(pxVkSwapChain); ++i) {
		vkDestroyFramebuffer(VkInstance_GetDevice(pxVkInstance), pxVkRenderer->pxFrameBuffers[i], 0);
	}

	free(pxVkRenderer->pxFrameBuffers);

	vkDestroyPipeline(VkInstance_GetDevice(pxVkInstance), pxVkRenderer->xGraphicsPipeline, 0);

	vkDestroyPipelineLayout(VkInstance_GetDevice(pxVkInstance), pxVkRenderer->xPipelineLayout, 0);

	vkDestroyRenderPass(VkInstance_GetDevice(pxVkInstance), pxVkRenderer->xRenderPass, 0);

	free(pxVkRenderer);
}

void VkRenderer_Draw(struct xVkRenderer_t* pxVkRenderer, struct xVkInstance_t* pxVkInstance, struct xVkSwapChain_t* pxVkSwapChain) {
	VK_CHECK(vkResetFences(VkInstance_GetDevice(pxVkInstance), 1, &pxVkRenderer->xInFlightFence));

	uint32_t nImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(VkInstance_GetDevice(pxVkInstance), VkSwapChain_GetSwapChain(pxVkSwapChain), UINT64_MAX, pxVkRenderer->xImageAvailableSemaphore, VK_NULL_HANDLE, &nImageIndex));

	VK_CHECK(vkResetCommandBuffer(pxVkRenderer->xCommandBuffer, 0));

	VkRenderer_RecordCommandBuffer(pxVkRenderer, nImageIndex);

	VkSemaphore axWaitSemaphores[] = { pxVkRenderer->xImageAvailableSemaphore };
	VkSemaphore axSignalSemaphores[] = { pxVkRenderer->xRenderFinishedSemaphore };
	VkPipelineStageFlags axWaitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSwapchainKHR axSwapChains[] = { VkSwapChain_GetSwapChain(pxVkSwapChain) };

	VkSubmitInfo xSubmitInfo;
	memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
	xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	xSubmitInfo.waitSemaphoreCount = ARRAY_LENGTH(axWaitSemaphores);
	xSubmitInfo.pWaitSemaphores = axWaitSemaphores;
	xSubmitInfo.pWaitDstStageMask = axWaitStages;
	xSubmitInfo.commandBufferCount = 1;
	xSubmitInfo.pCommandBuffers = &pxVkRenderer->xCommandBuffer;
	xSubmitInfo.signalSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xSubmitInfo.pSignalSemaphores = axSignalSemaphores;

	VK_CHECK(vkQueueSubmit(VkInstance_GetGraphicsQueue(pxVkInstance), 1, &xSubmitInfo, pxVkRenderer->xInFlightFence));

	VkPresentInfoKHR xPresentInfo;
	memset(&xPresentInfo, 0, sizeof(xPresentInfo));
	xPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	xPresentInfo.waitSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xPresentInfo.pWaitSemaphores = axSignalSemaphores;
	xPresentInfo.swapchainCount = ARRAY_LENGTH(axSwapChains);
	xPresentInfo.pSwapchains = axSwapChains;
	xPresentInfo.pImageIndices = &nImageIndex;

	VK_CHECK(vkQueuePresentKHR(VkInstance_GetPresentQueue(pxVkInstance), &xPresentInfo));

	VK_CHECK(vkWaitForFences(VkInstance_GetDevice(pxVkInstance), 1, &pxVkRenderer->xInFlightFence, VK_TRUE, UINT64_MAX));
}
