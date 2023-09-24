#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <platform/nativewindow.h>

#include <vulkan/instance.h>
#include <vulkan/computepipeline.h>

struct xComputePipeline_t {
	VkPipelineLayout xPipelineLayout;
	VkPipeline xPipeline;
};

struct xComputePipeline_t* ComputePipeline_Alloc(struct xInstance_t* pxInstance, VkShaderModule xCompModule, VkDescriptorSetLayout xDescriptorSetLayout, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantRangeCount) {
	struct xComputePipeline_t* pxComputePipeline = (struct xComputePipeline_t*)calloc(1, sizeof(struct xComputePipeline_t));

	VkPipelineShaderStageCreateInfo xCompShaderStageCreateInfo;
	memset(&xCompShaderStageCreateInfo, 0, sizeof(xCompShaderStageCreateInfo));
	xCompShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	xCompShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	xCompShaderStageCreateInfo.module = xCompModule;
	xCompShaderStageCreateInfo.pName = "main";

	VkPipelineLayoutCreateInfo xPipelineLayoutCreateInfo;
	memset(&xPipelineLayoutCreateInfo, 0, sizeof(xPipelineLayoutCreateInfo));
	xPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	xPipelineLayoutCreateInfo.setLayoutCount = 1;
	xPipelineLayoutCreateInfo.pSetLayouts = &xDescriptorSetLayout;
	xPipelineLayoutCreateInfo.pushConstantRangeCount = nPushConstantRangeCount;
	xPipelineLayoutCreateInfo.pPushConstantRanges = pxPushConstantRanges;

	VK_CHECK(vkCreatePipelineLayout(Instance_GetDevice(pxInstance), &xPipelineLayoutCreateInfo, 0, &pxComputePipeline->xPipelineLayout));

	VkComputePipelineCreateInfo xPipelineCreateInfo;
	memset(&xPipelineCreateInfo, 0, sizeof(xPipelineCreateInfo));
	xPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	xPipelineCreateInfo.layout = pxComputePipeline->xPipelineLayout;
	xPipelineCreateInfo.stage = xCompShaderStageCreateInfo;

	VK_CHECK(vkCreateComputePipelines(Instance_GetDevice(pxInstance), 0, 1, &xPipelineCreateInfo, 0, &pxComputePipeline->xPipeline));

	return pxComputePipeline;
}

void ComputePipeline_Free(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance) {
	vkDestroyPipeline(Instance_GetDevice(pxInstance), pxComputePipeline->xPipeline, 0);
	vkDestroyPipelineLayout(Instance_GetDevice(pxInstance), pxComputePipeline->xPipelineLayout, 0);

	free(pxComputePipeline);
}

VkPipelineLayout ComputePipeline_GetPipelineLayout(struct xComputePipeline_t* pxComputePipeline) {
	return pxComputePipeline->xPipelineLayout;
}

VkPipeline ComputePipeline_GetPipeline(struct xComputePipeline_t* pxComputePipeline) {
	return pxComputePipeline->xPipeline;
}
