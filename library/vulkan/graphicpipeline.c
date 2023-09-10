#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <platform/nativewindow.h>

#include <vulkan/instance.h>
#include <vulkan/swapchain.h>
#include <vulkan/graphicpipeline.h>
#include <vulkan/vertex.h>

struct xGraphicPipeline_t {
	VkPipelineLayout xPipelineLayout;
	VkPipeline xPipeline;
};

struct xGraphicPipeline_t* GraphicPipeline_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, VkShaderModule xVertModule, VkShaderModule xFragModule, VkVertexInputBindingDescription xVertexInputBindingDescription, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions, uint32_t nVertexInputAttributeDescriptionCount, VkDescriptorSetLayout xDescriptorSetLayout, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantCount) {
	struct xGraphicPipeline_t* pxGraphicPipeline = (struct xGraphicPipeline_t*)calloc(1, sizeof(struct xGraphicPipeline_t));

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
	xVertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	xVertexInputCreateInfo.pVertexBindingDescriptions = &xVertexInputBindingDescription;
	xVertexInputCreateInfo.vertexAttributeDescriptionCount = nVertexInputAttributeDescriptionCount;
	xVertexInputCreateInfo.pVertexAttributeDescriptions = pxVertexInputAttributeDescriptions;

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
	xPipelineLayoutCreateInfo.pSetLayouts = &xDescriptorSetLayout;
	xPipelineLayoutCreateInfo.pushConstantRangeCount = nPushConstantCount;
	xPipelineLayoutCreateInfo.pPushConstantRanges = pxPushConstantRanges;

	VK_CHECK(vkCreatePipelineLayout(Instance_GetDevice(pxInstance), &xPipelineLayoutCreateInfo, 0, &pxGraphicPipeline->xPipelineLayout));

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
	xPipelineCreateInfo.layout = pxGraphicPipeline->xPipelineLayout;
	xPipelineCreateInfo.renderPass = SwapChain_GetRenderPass(pxSwapChain);
	xPipelineCreateInfo.subpass = 0;
	xPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;

	VK_CHECK(vkCreateGraphicsPipelines(Instance_GetDevice(pxInstance), VK_NULL_HANDLE, 1, &xPipelineCreateInfo, 0, &pxGraphicPipeline->xPipeline));

	return pxGraphicPipeline;
}

void GraphicPipeline_Free(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance) {
	vkDestroyPipeline(Instance_GetDevice(pxInstance), pxGraphicPipeline->xPipeline, 0);
	vkDestroyPipelineLayout(Instance_GetDevice(pxInstance), pxGraphicPipeline->xPipelineLayout, 0);

	free(pxGraphicPipeline);
}

VkPipelineLayout GraphicPipeline_GetPipelineLayout(struct xGraphicPipeline_t* pxGraphicPipeline) {
	return pxGraphicPipeline->xPipelineLayout;
}

VkPipeline GraphicPipeline_GetPipeline(struct xGraphicPipeline_t* pxGraphicPipeline) {
	return pxGraphicPipeline->xPipeline;
}
