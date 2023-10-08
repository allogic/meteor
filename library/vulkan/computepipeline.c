#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <container/list.h>
#include <container/vector.h>

#include <ecs/entity.h>

#include <platform/nativewindow.h>

#include <vulkan/common.h>
#include <vulkan/computepipeline.h>
#include <vulkan/instance.h>
#include <vulkan/shader.h>

#define PIPELINE_NAME_LENGTH 32

struct xComputePipeline_t {
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

static void ComputePipeline_AllocDescriptorPool(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance, uint32_t nPoolCount, VkDescriptorSetLayoutBinding* pxDescriptorSetLayoutBindings, uint32_t nDescriptorSetLayoutBindingCount) {
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

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxComputePipeline->xDescriptorPool));

	Vector_Free(pxDescriptorPoolSizes);
}
static void ComputePipeline_AllocDescriptorSetLayout(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance, VkDescriptorSetLayoutBinding* pxDescriptorSetLayoutBindings, uint32_t nDescriptorSetLayoutBindingCount) {
	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = nDescriptorSetLayoutBindingCount;
	xDescriptorSetLayoutCreateInfo.pBindings = pxDescriptorSetLayoutBindings;

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxComputePipeline->xDescriptorSetLayout));
}
static void ComputePipeline_AllocPipelineLayout(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantRangeCount) {
	VkPipelineLayoutCreateInfo xPipelineLayoutCreateInfo;
	memset(&xPipelineLayoutCreateInfo, 0, sizeof(xPipelineLayoutCreateInfo));
	xPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	xPipelineLayoutCreateInfo.setLayoutCount = 1;
	xPipelineLayoutCreateInfo.pSetLayouts = &pxComputePipeline->xDescriptorSetLayout;
	xPipelineLayoutCreateInfo.pushConstantRangeCount = nPushConstantRangeCount;
	xPipelineLayoutCreateInfo.pPushConstantRanges = pxPushConstantRanges;

	VK_CHECK(vkCreatePipelineLayout(Instance_GetDevice(pxInstance), &xPipelineLayoutCreateInfo, 0, &pxComputePipeline->xPipelineLayout));
}
static void ComputePipeline_AllocPipeline(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance, const char* pcComputeFilePath) {
	VkShaderModule xComputeModule = Shader_Alloc(pxInstance, pcComputeFilePath);

	VkPipelineShaderStageCreateInfo xCompShaderStageCreateInfo;
	memset(&xCompShaderStageCreateInfo, 0, sizeof(xCompShaderStageCreateInfo));
	xCompShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	xCompShaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	xCompShaderStageCreateInfo.module = xComputeModule;
	xCompShaderStageCreateInfo.pName = "main";

	VkComputePipelineCreateInfo xPipelineCreateInfo;
	memset(&xPipelineCreateInfo, 0, sizeof(xPipelineCreateInfo));
	xPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	xPipelineCreateInfo.layout = pxComputePipeline->xPipelineLayout;
	xPipelineCreateInfo.stage = xCompShaderStageCreateInfo;

	VK_CHECK(vkCreateComputePipelines(Instance_GetDevice(pxInstance), 0, 1, &xPipelineCreateInfo, 0, &pxComputePipeline->xPipeline));

	Shader_Free(pxInstance, xComputeModule);
}

struct xComputePipeline_t* ComputePipeline_Alloc(struct xInstance_t* pxInstance, const char* pcName, xMaterialMapHandler_t pMaterialMapHandler, uint32_t nPoolCount, const char* pcComputeFilePath, VkDescriptorSetLayoutBinding* pxDescriptorSetLayoutBindings, uint32_t nDescriptorSetLayoutBindingCount, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantRangeCount) {
	struct xComputePipeline_t* pxComputePipeline = (struct xComputePipeline_t*)calloc(1, sizeof(struct xComputePipeline_t));

	uint32_t nNameLength = strlen(pcName);
	memcpy(pxComputePipeline->acName, pcName, MIN(PIPELINE_NAME_LENGTH - 1, nNameLength));
	pxComputePipeline->acName[PIPELINE_NAME_LENGTH - 1] = 0;

	pxComputePipeline->pMaterialMapHandler = pMaterialMapHandler;

	pxComputePipeline->pxDescriptorSetLayouts = Vector_Alloc(sizeof(VkDescriptorSetLayout));
	pxComputePipeline->pxDescriptorSets = Vector_Alloc(sizeof(VkDescriptorSet));
	pxComputePipeline->pxEntities = List_Alloc(sizeof(struct xEntity_t*));

	ComputePipeline_AllocDescriptorPool(pxComputePipeline, pxInstance, nPoolCount, pxDescriptorSetLayoutBindings, nDescriptorSetLayoutBindingCount);
	ComputePipeline_AllocDescriptorSetLayout(pxComputePipeline, pxInstance, pxDescriptorSetLayoutBindings, nDescriptorSetLayoutBindingCount);
	ComputePipeline_AllocPipelineLayout(pxComputePipeline, pxInstance, pxPushConstantRanges, nPushConstantRangeCount);
	ComputePipeline_AllocPipeline(pxComputePipeline, pxInstance, pcComputeFilePath);

	for (uint32_t i = 0; i < nPoolCount; ++i) {
		Vector_Push(pxComputePipeline->pxDescriptorSetLayouts, &pxComputePipeline->xDescriptorSetLayout);
	}

	return pxComputePipeline;
}
void ComputePipeline_Free(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance) {
	vkDestroyPipeline(Instance_GetDevice(pxInstance), pxComputePipeline->xPipeline, 0);
	vkDestroyPipelineLayout(Instance_GetDevice(pxInstance), pxComputePipeline->xPipelineLayout, 0);
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxComputePipeline->xDescriptorSetLayout, 0);
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxComputePipeline->xDescriptorPool, 0);

	List_Free(pxComputePipeline->pxEntities);
	Vector_Free(pxComputePipeline->pxDescriptorSets);
	Vector_Free(pxComputePipeline->pxDescriptorSetLayouts);

	free(pxComputePipeline);
}

const char* ComputePipeline_GetName(struct xComputePipeline_t* pxComputePipeline) {
	return pxComputePipeline->acName;
}
VkDescriptorPool ComputePipeline_GetDescriptorPool(struct xComputePipeline_t* pxComputePipeline) {
	return pxComputePipeline->xDescriptorPool;
}
VkDescriptorSetLayout ComputePipeline_GetDescriptorSetLayout(struct xComputePipeline_t* pxComputePipeline) {
	return pxComputePipeline->xDescriptorSetLayout;
}
VkPipelineLayout ComputePipeline_GetPipelineLayout(struct xComputePipeline_t* pxComputePipeline) {
	return pxComputePipeline->xPipelineLayout;
}
VkPipeline ComputePipeline_GetPipeline(struct xComputePipeline_t* pxComputePipeline) {
	return pxComputePipeline->xPipeline;
}
struct xList_t* ComputePipeline_GetEntities(struct xComputePipeline_t* pxComputePipeline) {
	return pxComputePipeline->pxEntities;
}

void ComputePipeline_CreateDescriptorSets(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance) {
	uint32_t nDescriptorCount = List_Count(pxComputePipeline->pxEntities);

	Vector_Resize(pxComputePipeline->pxDescriptorSets, nDescriptorCount);

	if (nDescriptorCount) {
		VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
		memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
		xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		xDescriptorSetAllocateInfo.descriptorPool = pxComputePipeline->xDescriptorPool;
		xDescriptorSetAllocateInfo.descriptorSetCount = nDescriptorCount;
		xDescriptorSetAllocateInfo.pSetLayouts = Vector_Data(pxComputePipeline->pxDescriptorSetLayouts);

		VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, Vector_Data(pxComputePipeline->pxDescriptorSets)));
	}
}
void ComputePipeline_UpdateDescriptorSets(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance) {
	uint32_t nDescriptorSetIndex = 0;

	void* pxEntityIter = List_Begin(pxComputePipeline->pxEntities);
	while (pxEntityIter) {
		struct xEntity_t* pxEntity = *(struct xEntity_t**)List_Value(pxEntityIter);

		xComputable_t* pxComputable = Entity_GetComputable(pxEntity);

		if (pxComputePipeline->pMaterialMapHandler) {
			pxComputePipeline->pMaterialMapHandler(pxComputable->pMaterial);

			VkWriteDescriptorSet axWriteDescriptorSets[3];

			//axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			//axWriteDescriptorSets[0].pNext = 0;
			//axWriteDescriptorSets[0].dstSet = *(VkDescriptorSet*)Vector_At(pxComputePipeline->pxDescriptorSets, nDescriptorSetIndex);
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
			//axWriteDescriptorSets[1].dstSet = *(VkDescriptorSet*)Vector_At(pxComputePipeline->pxDescriptorSets, nDescriptorSetIndex);
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
			//axWriteDescriptorSets[2].dstSet = *(VkDescriptorSet*)Vector_At(pxComputePipeline->pxDescriptorSets, nDescriptorSetIndex);
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

void ComputePipeline_PushEntity(struct xComputePipeline_t* pxComputePipeline, struct xEntity_t* pxEntity) {
	List_Push(pxComputePipeline->pxEntities, &pxEntity);
}
