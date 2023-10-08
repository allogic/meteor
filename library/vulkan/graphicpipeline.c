#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <container/list.h>
#include <container/vector.h>

#include <ecs/entity.h>

#include <platform/nativewindow.h>

#include <vulkan/common.h>
#include <vulkan/graphicpipeline.h>
#include <vulkan/instance.h>
#include <vulkan/swapchain.h>
#include <vulkan/shader.h>

#define PIPELINE_NAME_LENGTH 32

struct xGraphicPipeline_t {
	char acName[PIPELINE_NAME_LENGTH];
	xMaterialMapHandler_t pMaterialMapHandler;
	VkDescriptorPool xDescriptorPool;
	VkDescriptorSetLayout xDescriptorSetLayout;
	VkPipelineLayout xPipelineLayout;
	VkPipeline xPipeline;
	struct xVector_t* pxDescriptorSetLayouts;
	struct xVector_t* pxDescriptorSets;
	struct xList_t* pxEntities;
};

static void GraphicPipeline_AllocDescriptorPool(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance, uint32_t nPoolCount, VkDescriptorSetLayoutBinding* pxDescriptorSetLayoutBindings, uint32_t nDescriptorSetLayoutBindingCount) {
	uint32_t anDescriptorTypeCounts[MAX_DESCRIPTOR_TYPE];
	memset(anDescriptorTypeCounts, 0, sizeof(anDescriptorTypeCounts));

	for (uint32_t i = 0; i < nDescriptorSetLayoutBindingCount; ++i) {
		anDescriptorTypeCounts[pxDescriptorSetLayoutBindings[i].descriptorType] += 1;
	}

	struct xVector_t* pxDescriptorPoolSizes = Vector_Alloc(sizeof(VkDescriptorPoolSize));

	VkDescriptorPoolSize xDescriptorPoolSize;
	for (uint32_t i = 0; i < MAX_DESCRIPTOR_TYPE; ++i) {
		if (anDescriptorTypeCounts[i] > 0) {
			xDescriptorPoolSize.type = i;
			xDescriptorPoolSize.descriptorCount = anDescriptorTypeCounts[i];

			Vector_Push(pxDescriptorPoolSizes, &xDescriptorPoolSize);
		}
	}

	VkDescriptorPoolCreateInfo xDescriptorPoolCreateInfo;
	memset(&xDescriptorPoolCreateInfo, 0, sizeof(xDescriptorPoolCreateInfo));
	xDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	xDescriptorPoolCreateInfo.poolSizeCount = Vector_Count(pxDescriptorPoolSizes);
	xDescriptorPoolCreateInfo.pPoolSizes = Vector_Data(pxDescriptorPoolSizes);
	xDescriptorPoolCreateInfo.maxSets = nPoolCount;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxGraphicPipeline->xDescriptorPool));

	Vector_Free(pxDescriptorPoolSizes);
}
static void GraphicPipeline_AllocDescriptorSetLayout(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance, VkDescriptorSetLayoutBinding* pxDescriptorSetLayoutBindings, uint32_t nDescriptorSetLayoutBindingCount) {
	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = nDescriptorSetLayoutBindingCount;
	xDescriptorSetLayoutCreateInfo.pBindings = pxDescriptorSetLayoutBindings;

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxGraphicPipeline->xDescriptorSetLayout));
}
static void GraphicPipeline_AllocPipelineLayout(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantRangeCount) {
	VkPipelineLayoutCreateInfo xPipelineLayoutCreateInfo;
	memset(&xPipelineLayoutCreateInfo, 0, sizeof(xPipelineLayoutCreateInfo));
	xPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	xPipelineLayoutCreateInfo.setLayoutCount = 1;
	xPipelineLayoutCreateInfo.pSetLayouts = &pxGraphicPipeline->xDescriptorSetLayout;
	xPipelineLayoutCreateInfo.pushConstantRangeCount = nPushConstantRangeCount;
	xPipelineLayoutCreateInfo.pPushConstantRanges = pxPushConstantRanges;

	VK_CHECK(vkCreatePipelineLayout(Instance_GetDevice(pxInstance), &xPipelineLayoutCreateInfo, 0, &pxGraphicPipeline->xPipelineLayout));
}
static void GraphicPipeline_AllocPipeline(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, VkPrimitiveTopology xPrimitiveTopology, const char* pcVertexFilePath, const char* pcFragmentFilePath, VkVertexInputBindingDescription* pxVertexInputBindingDescriptions, uint32_t nVertexInputBindingDescriptionCount, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions, uint32_t nVertexInputAttributeDescriptionCount) {
	VkShaderModule xVertexModule = Shader_Alloc(pxInstance, pcVertexFilePath);
	VkShaderModule xFragmentModule = Shader_Alloc(pxInstance, pcFragmentFilePath);

	VkPipelineShaderStageCreateInfo xVertShaderStageCreateInfo;
	memset(&xVertShaderStageCreateInfo, 0, sizeof(xVertShaderStageCreateInfo));
	xVertShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	xVertShaderStageCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	xVertShaderStageCreateInfo.module = xVertexModule;
	xVertShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo xFragShaderStageCreateInfo;
	memset(&xFragShaderStageCreateInfo, 0, sizeof(xFragShaderStageCreateInfo));
	xFragShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	xFragShaderStageCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	xFragShaderStageCreateInfo.module = xFragmentModule;
	xFragShaderStageCreateInfo.pName = "main";

	VkPipelineShaderStageCreateInfo axShaderStages[] = { xVertShaderStageCreateInfo, xFragShaderStageCreateInfo };

	VkPipelineVertexInputStateCreateInfo xVertexInputCreateInfo;
	memset(&xVertexInputCreateInfo, 0, sizeof(xVertexInputCreateInfo));
	xVertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	xVertexInputCreateInfo.vertexBindingDescriptionCount = nVertexInputBindingDescriptionCount;
	xVertexInputCreateInfo.pVertexBindingDescriptions = pxVertexInputBindingDescriptions;
	xVertexInputCreateInfo.vertexAttributeDescriptionCount = nVertexInputAttributeDescriptionCount;
	xVertexInputCreateInfo.pVertexAttributeDescriptions = pxVertexInputAttributeDescriptions;

	VkPipelineInputAssemblyStateCreateInfo xInputAssemblyCreateInfo;
	memset(&xInputAssemblyCreateInfo, 0, sizeof(xInputAssemblyCreateInfo));
	xInputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	xInputAssemblyCreateInfo.topology = xPrimitiveTopology;
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
	xPipelineCreateInfo.basePipelineHandle = 0;

	VK_CHECK(vkCreateGraphicsPipelines(Instance_GetDevice(pxInstance), 0, 1, &xPipelineCreateInfo, 0, &pxGraphicPipeline->xPipeline));

	Shader_Free(pxInstance, xVertexModule);
	Shader_Free(pxInstance, xFragmentModule);
}

struct xGraphicPipeline_t* GraphicPipeline_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, const char* pcName, xMaterialMapHandler_t pMaterialMapHandler, VkPrimitiveTopology xPrimitiveTopology, uint32_t nPoolCount, const char* pcVertexFilePath, const char* pcFragmentFilePath, VkDescriptorSetLayoutBinding* pxDescriptorSetLayoutBindings, uint32_t nDescriptorSetLayoutBindingCount, VkVertexInputBindingDescription* pxVertexInputBindingDescriptions, uint32_t nVertexInputBindingDescriptionCount, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions, uint32_t nVertexInputAttributeDescriptionCount, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantRangeCount) {
	struct xGraphicPipeline_t* pxGraphicPipeline = (struct xGraphicPipeline_t*)calloc(1, sizeof(struct xGraphicPipeline_t));

	uint32_t nNameLength = strlen(pcName);
	memcpy(pxGraphicPipeline->acName, pcName, MIN(PIPELINE_NAME_LENGTH - 1, nNameLength));
	pxGraphicPipeline->acName[PIPELINE_NAME_LENGTH - 1] = 0;

	pxGraphicPipeline->pMaterialMapHandler = pMaterialMapHandler;

	pxGraphicPipeline->pxDescriptorSetLayouts = Vector_Alloc(sizeof(VkDescriptorSetLayout));
	pxGraphicPipeline->pxDescriptorSets = Vector_Alloc(sizeof(VkDescriptorSet));
	pxGraphicPipeline->pxEntities = List_Alloc(sizeof(struct xEntity_t*));

	GraphicPipeline_AllocDescriptorPool(pxGraphicPipeline, pxInstance, nPoolCount, pxDescriptorSetLayoutBindings, nDescriptorSetLayoutBindingCount);
	GraphicPipeline_AllocDescriptorSetLayout(pxGraphicPipeline, pxInstance, pxDescriptorSetLayoutBindings, nDescriptorSetLayoutBindingCount);
	GraphicPipeline_AllocPipelineLayout(pxGraphicPipeline, pxInstance, pxPushConstantRanges, nPushConstantRangeCount);
	GraphicPipeline_AllocPipeline(pxGraphicPipeline, pxInstance, pxSwapChain, xPrimitiveTopology, pcVertexFilePath, pcFragmentFilePath, pxVertexInputBindingDescriptions, nVertexInputBindingDescriptionCount, pxVertexInputAttributeDescriptions, nVertexInputAttributeDescriptionCount);

	for (uint32_t i = 0; i < nPoolCount; ++i) {
		Vector_Push(pxGraphicPipeline->pxDescriptorSetLayouts, &pxGraphicPipeline->xDescriptorSetLayout);
	}

	return pxGraphicPipeline;
}
void GraphicPipeline_Free(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance) {
	vkDestroyPipeline(Instance_GetDevice(pxInstance), pxGraphicPipeline->xPipeline, 0);
	vkDestroyPipelineLayout(Instance_GetDevice(pxInstance), pxGraphicPipeline->xPipelineLayout, 0);
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxGraphicPipeline->xDescriptorSetLayout, 0);
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxGraphicPipeline->xDescriptorPool, 0);

	List_Free(pxGraphicPipeline->pxEntities);
	Vector_Free(pxGraphicPipeline->pxDescriptorSets);
	Vector_Free(pxGraphicPipeline->pxDescriptorSetLayouts);

	free(pxGraphicPipeline);
}

const char* GraphicPipeline_GetName(struct xGraphicPipeline_t* pxGraphicPipeline) {
	return pxGraphicPipeline->acName;
}
VkDescriptorPool GraphicPipeline_GetDescriptorPool(struct xGraphicPipeline_t* pxGraphicPipeline) {
	return pxGraphicPipeline->xDescriptorPool;
}
VkDescriptorSetLayout GraphicPipeline_GetDescriptorSetLayout(struct xGraphicPipeline_t* pxGraphicPipeline) {
	return pxGraphicPipeline->xDescriptorSetLayout;
}
VkPipelineLayout GraphicPipeline_GetPipelineLayout(struct xGraphicPipeline_t* pxGraphicPipeline) {
	return pxGraphicPipeline->xPipelineLayout;
}
VkPipeline GraphicPipeline_GetPipeline(struct xGraphicPipeline_t* pxGraphicPipeline) {
	return pxGraphicPipeline->xPipeline;
}
struct xList_t* GraphicPipeline_GetEntities(struct xGraphicPipeline_t* pxGraphicPipeline) {
	return pxGraphicPipeline->pxEntities;
}

void GraphicPipeline_CreateDescriptorSets(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance) {
	uint32_t nDescriptorCount = List_Count(pxGraphicPipeline->pxEntities);

	Vector_Resize(pxGraphicPipeline->pxDescriptorSets, nDescriptorCount);

	if (nDescriptorCount) {
		VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
		memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
		xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		xDescriptorSetAllocateInfo.descriptorPool = pxGraphicPipeline->xDescriptorPool;
		xDescriptorSetAllocateInfo.descriptorSetCount = nDescriptorCount;
		xDescriptorSetAllocateInfo.pSetLayouts = Vector_Data(pxGraphicPipeline->pxDescriptorSetLayouts);

		VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, Vector_Data(pxGraphicPipeline->pxDescriptorSets)));
	}
}
void GraphicPipeline_UpdateDescriptorSets(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance) {
	uint32_t nDescriptorSetIndex = 0;

	struct xVector_t* pxDescriptorInfos = Vector_Alloc(sizeof());

	void* pxEntityIter = List_Begin(pxGraphicPipeline->pxEntities);
	while (pxEntityIter) {
		struct xEntity_t* pxEntity = *(struct xEntity_t**)List_Value(pxEntityIter);

		xRenderable_t* pxRenderable = Entity_GetRenderable(pxEntity);

		if (pxGraphicPipeline->pMaterialMapHandler) {
			pxGraphicPipeline->pMaterialMapHandler(pxRenderable->pMaterial);

			VkWriteDescriptorSet axWriteDescriptorSets[3];

			//axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//axWriteDescriptorSets[0].pNext = 0;
			//axWriteDescriptorSets[0].dstSet = *(VkDescriptorSet*)Vector_At(pxGraphicPipeline->pxDescriptorSets, nDescriptorSetIndex);
			//axWriteDescriptorSets[0].dstBinding = 0;
			//axWriteDescriptorSets[0].dstArrayElement = 0;
			//axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			//axWriteDescriptorSets[0].descriptorCount = 1;
			//axWriteDescriptorSets[0].pImageInfo = 0;
			//axWriteDescriptorSets[0].pBufferInfo = &xViewProjectionDescriptorBufferInfo;
			//axWriteDescriptorSets[0].pTexelBufferView = 0;
//
			//axWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//axWriteDescriptorSets[1].pNext = 0;
			//axWriteDescriptorSets[1].dstSet = *(VkDescriptorSet*)Vector_At(pxGraphicPipeline->pxDescriptorSets, nDescriptorSetIndex);
			//axWriteDescriptorSets[1].dstBinding = 1;
			//axWriteDescriptorSets[1].dstArrayElement = 0;
			//axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			//axWriteDescriptorSets[1].descriptorCount = 1;
			//axWriteDescriptorSets[1].pImageInfo = 0;
			//axWriteDescriptorSets[1].pBufferInfo = &xTimeInfoDescriptorBufferInfo;
			//axWriteDescriptorSets[1].pTexelBufferView = 0;
//
			//axWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//axWriteDescriptorSets[2].pNext = 0;
			//axWriteDescriptorSets[2].dstSet = *(VkDescriptorSet*)Vector_At(pxGraphicPipeline->pxDescriptorSets, nDescriptorSetIndex);
			//axWriteDescriptorSets[2].dstBinding = 2;
			//axWriteDescriptorSets[2].dstArrayElement = 0;
			//axWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			//axWriteDescriptorSets[2].descriptorCount = 1;
			//axWriteDescriptorSets[2].pImageInfo = &xCombinedImageSamplerDescriptorImageInfo;
			//axWriteDescriptorSets[2].pBufferInfo = 0;
			//axWriteDescriptorSets[2].pTexelBufferView = 0;

			//vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);

			nDescriptorSetIndex += 1;
		}

		pxEntityIter = List_Next(pxEntityIter);
	}
}

void GraphicPipeline_PushEntity(struct xGraphicPipeline_t* pxGraphicPipeline, struct xEntity_t* pxEntity) {
	List_Push(pxGraphicPipeline->pxEntities, &pxEntity);
}
