#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <math/matrix.h>

#include <standard/list.h>

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

#include <game/assets.h>
#include <game/renderer.h>
#include <game/component.h>
#include <game/entity.h>

struct xRenderer_t {
	xTimeInfo_t xTimeInfo;
	xViewProjection_t xViewProjection;

	struct xBuffer_t* pxTimeInfoBuffer;
	struct xBuffer_t* pxViewProjectionBuffer;

	VkDescriptorSetLayout xDefaultGraphicDescriptorSetLayout;
	VkDescriptorPool xDefaultGraphicDescriptorPool;
	VkDescriptorSet xDefaultGraphicDescriptorSet;
	VkPushConstantRange axDefaultGraphicPushConstantRanges[1];
	VkSampler axDefaultGraphicSampler;
	struct xGraphicPipeline_t* pxDefaultGraphicPipeline;

	VkDescriptorSetLayout xParticleGraphicDescriptorSetLayout;
	VkDescriptorSetLayout xParticleComputeDescriptorSetLayout;
	VkDescriptorPool xParticleGraphicDescriptorPool;
	VkDescriptorPool xParticleComputeDescriptorPool;
	VkDescriptorSet xParticleGraphicDescriptorSet;
	VkDescriptorSet xParticleComputeDescriptorSet;
	VkPushConstantRange axParticleGraphicPushConstantRanges[1];
	VkSampler axParticleGraphicSampler;
	struct xGraphicPipeline_t* pxParticleGraphicPipeline;
	struct xComputePipeline_t* pxParticleComputePipeline;

	VkCommandBuffer xGraphicCommandBuffer;
	VkCommandBuffer xComputeCommandBuffer;
	VkSemaphore xImageAvailableSemaphore;
	VkSemaphore xGraphicFinishedSemaphore;
	VkSemaphore xComputeFinishedSemaphore;
	VkFence xGraphicInFlightFence;
	VkFence xComputeInFlightFence;
};

static void Renderer_CreateDefaultGraphicDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[3];

	axDescriptorSetLayoutBindings[0].binding = 0;
	axDescriptorSetLayoutBindings[0].descriptorCount = 1;
	axDescriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorSetLayoutBindings[0].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	axDescriptorSetLayoutBindings[1].binding = 1;
	axDescriptorSetLayoutBindings[1].descriptorCount = 1;
	axDescriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	axDescriptorSetLayoutBindings[1].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	axDescriptorSetLayoutBindings[2].binding = 2;
	axDescriptorSetLayoutBindings[2].descriptorCount = g_nImageAssetsCount;
	axDescriptorSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	axDescriptorSetLayoutBindings[2].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = ARRAY_LENGTH(axDescriptorSetLayoutBindings);
	xDescriptorSetLayoutCreateInfo.pBindings = axDescriptorSetLayoutBindings;

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xDefaultGraphicDescriptorSetLayout));
}

static void Renderer_CreateParticleGraphicDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[4];

	axDescriptorSetLayoutBindings[0].binding = 0;
	axDescriptorSetLayoutBindings[0].descriptorCount = 1;
	axDescriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorSetLayoutBindings[0].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	axDescriptorSetLayoutBindings[1].binding = 1;
	axDescriptorSetLayoutBindings[1].descriptorCount = 1;
	axDescriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axDescriptorSetLayoutBindings[1].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	axDescriptorSetLayoutBindings[2].binding = 2;
	axDescriptorSetLayoutBindings[2].descriptorCount = 1;
	axDescriptorSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	axDescriptorSetLayoutBindings[2].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	axDescriptorSetLayoutBindings[3].binding = 3;
	axDescriptorSetLayoutBindings[3].descriptorCount = g_nImageAssetsCount;
	axDescriptorSetLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	axDescriptorSetLayoutBindings[3].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = ARRAY_LENGTH(axDescriptorSetLayoutBindings);
	xDescriptorSetLayoutCreateInfo.pBindings = axDescriptorSetLayoutBindings;

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xParticleGraphicDescriptorSetLayout));
}

static void Renderer_CreateParticleComputeDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
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

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xParticleComputeDescriptorSetLayout));
}

static void Renderer_CreateDefaultGraphicDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorPoolSize axDescriptorPoolSizes[3];

	axDescriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorPoolSizes[0].descriptorCount = 1;

	axDescriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	axDescriptorPoolSizes[1].descriptorCount = 1;

	axDescriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	axDescriptorPoolSizes[2].descriptorCount = g_nImageAssetsCount;

	VkDescriptorPoolCreateInfo xDescriptorPoolCreateInfo;
	memset(&xDescriptorPoolCreateInfo, 0, sizeof(xDescriptorPoolCreateInfo));
	xDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	xDescriptorPoolCreateInfo.poolSizeCount = ARRAY_LENGTH(axDescriptorPoolSizes);
	xDescriptorPoolCreateInfo.pPoolSizes = axDescriptorPoolSizes;
	xDescriptorPoolCreateInfo.maxSets = 1;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xDefaultGraphicDescriptorPool));
}

static void Renderer_CreateParticleGraphicDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorPoolSize axDescriptorPoolSizes[4];

	axDescriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorPoolSizes[0].descriptorCount = 1;

	axDescriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axDescriptorPoolSizes[1].descriptorCount = 1;

	axDescriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	axDescriptorPoolSizes[2].descriptorCount = 1;

	axDescriptorPoolSizes[3].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	axDescriptorPoolSizes[3].descriptorCount = g_nImageAssetsCount;

	VkDescriptorPoolCreateInfo xDescriptorPoolCreateInfo;
	memset(&xDescriptorPoolCreateInfo, 0, sizeof(xDescriptorPoolCreateInfo));
	xDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	xDescriptorPoolCreateInfo.poolSizeCount = ARRAY_LENGTH(axDescriptorPoolSizes);
	xDescriptorPoolCreateInfo.pPoolSizes = axDescriptorPoolSizes;
	xDescriptorPoolCreateInfo.maxSets = 1;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xParticleGraphicDescriptorPool));
}

static void Renderer_CreateParticleComputeDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
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

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xParticleComputeDescriptorPool));
}

static void Renderer_CreateDefaultGraphicDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayout axDescriptorSetsLayouts[] = { pxRenderer->xDefaultGraphicDescriptorSetLayout };

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xDefaultGraphicDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = 1;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetsLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, &pxRenderer->xDefaultGraphicDescriptorSet));
}

static void Renderer_CreateParticleGraphicDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayout axDescriptorSetsLayouts[] = { pxRenderer->xParticleGraphicDescriptorSetLayout };

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xParticleGraphicDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = 1;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetsLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, &pxRenderer->xParticleGraphicDescriptorSet));
}

static void Renderer_CreateParticleComputeDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayout axDescriptorSetsLayouts[] = { pxRenderer->xParticleComputeDescriptorSetLayout };

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xParticleComputeDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = 1;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetsLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, &pxRenderer->xParticleComputeDescriptorSet));
}

static void Renderer_CreateDefaultGraphicSampler(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkPhysicalDeviceProperties xPhysicalDeviceProperties;
	vkGetPhysicalDeviceProperties(Instance_GetPhysicalDevice(pxInstance), &xPhysicalDeviceProperties);

	VkSamplerCreateInfo xSamplerCreateInfo;
	memset(&xSamplerCreateInfo, 0, sizeof(xSamplerCreateInfo));
	xSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	xSamplerCreateInfo.magFilter = VK_FILTER_NEAREST;
	xSamplerCreateInfo.minFilter = VK_FILTER_NEAREST;
	xSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	xSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	xSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	xSamplerCreateInfo.anisotropyEnable = VK_TRUE;
	xSamplerCreateInfo.maxAnisotropy = xPhysicalDeviceProperties.limits.maxSamplerAnisotropy;
	xSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	xSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	xSamplerCreateInfo.compareEnable = VK_FALSE;
	xSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	xSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	xSamplerCreateInfo.mipLodBias = 0.0F;
	xSamplerCreateInfo.minLod = 0.0F;
	xSamplerCreateInfo.maxLod = 0.0F;

	VK_CHECK(vkCreateSampler(Instance_GetDevice(pxInstance), &xSamplerCreateInfo, 0, &pxRenderer->axDefaultGraphicSampler));
}

static void Renderer_CreateParticleGraphicSampler(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkPhysicalDeviceProperties xPhysicalDeviceProperties;
	vkGetPhysicalDeviceProperties(Instance_GetPhysicalDevice(pxInstance), &xPhysicalDeviceProperties);

	VkSamplerCreateInfo xSamplerCreateInfo;
	memset(&xSamplerCreateInfo, 0, sizeof(xSamplerCreateInfo));
	xSamplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	xSamplerCreateInfo.magFilter = VK_FILTER_NEAREST;
	xSamplerCreateInfo.minFilter = VK_FILTER_NEAREST;
	xSamplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	xSamplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	xSamplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	xSamplerCreateInfo.anisotropyEnable = VK_TRUE;
	xSamplerCreateInfo.maxAnisotropy = xPhysicalDeviceProperties.limits.maxSamplerAnisotropy;
	xSamplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	xSamplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
	xSamplerCreateInfo.compareEnable = VK_FALSE;
	xSamplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	xSamplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	xSamplerCreateInfo.mipLodBias = 0.0F;
	xSamplerCreateInfo.minLod = 0.0F;
	xSamplerCreateInfo.maxLod = 0.0F;

	VK_CHECK(vkCreateSampler(Instance_GetDevice(pxInstance), &xSamplerCreateInfo, 0, &pxRenderer->axParticleGraphicSampler));
}

static void Renderer_UpdateDefaultGraphicDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorBufferInfo xViewProjectionDescriptorBufferInfo;
	memset(&xViewProjectionDescriptorBufferInfo, 0, sizeof(xViewProjectionDescriptorBufferInfo));
	xViewProjectionDescriptorBufferInfo.offset = 0;
	xViewProjectionDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxViewProjectionBuffer);
	xViewProjectionDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxViewProjectionBuffer);

	VkDescriptorImageInfo xSamplerDescriptorImageInfo;
	memset(&xSamplerDescriptorImageInfo, 0, sizeof(xSamplerDescriptorImageInfo));
	xSamplerDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//xSamplerDescriptorImageInfo.imageView = Image_GetImageView(pxAlbedoImage); // TODO
	xSamplerDescriptorImageInfo.sampler = pxRenderer->axDefaultGraphicSampler;

	VkDescriptorImageInfo axSampledImageDescriptorImageInfo[g_nImageAssetsCount];
	memset(axSampledImageDescriptorImageInfo, 0, sizeof(axSampledImageDescriptorImageInfo));

	for (uint32_t i = 0; i < g_nImageAssetsCount; ++i) {
		axSampledImageDescriptorImageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		axSampledImageDescriptorImageInfo[i].imageView = Image_GetImageView(Assets_GetImageByIndex(i));
		axSampledImageDescriptorImageInfo[i].sampler = 0;
	}

	VkWriteDescriptorSet axWriteDescriptorSets[3];

	axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[0].pNext = 0;
	axWriteDescriptorSets[0].dstSet = pxRenderer->xDefaultGraphicDescriptorSet;
	axWriteDescriptorSets[0].dstBinding = 0;
	axWriteDescriptorSets[0].dstArrayElement = 0;
	axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[0].descriptorCount = 1;
	axWriteDescriptorSets[0].pImageInfo = 0;
	axWriteDescriptorSets[0].pBufferInfo = &xViewProjectionDescriptorBufferInfo;
	axWriteDescriptorSets[0].pTexelBufferView = 0;

	axWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[1].pNext = 0;
	axWriteDescriptorSets[1].dstSet = pxRenderer->xDefaultGraphicDescriptorSet;
	axWriteDescriptorSets[1].dstBinding = 1;
	axWriteDescriptorSets[1].dstArrayElement = 0;
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = &xSamplerDescriptorImageInfo;
	axWriteDescriptorSets[1].pBufferInfo = 0;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	axWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[2].pNext = 0;
	axWriteDescriptorSets[2].dstSet = pxRenderer->xDefaultGraphicDescriptorSet;
	axWriteDescriptorSets[2].dstBinding = 2;
	axWriteDescriptorSets[2].dstArrayElement = 0;
	axWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	axWriteDescriptorSets[2].descriptorCount = g_nImageAssetsCount;
	axWriteDescriptorSets[2].pImageInfo = axSampledImageDescriptorImageInfo;
	axWriteDescriptorSets[2].pBufferInfo = 0;
	axWriteDescriptorSets[2].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}

static void Renderer_UpdateParticleGraphicDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorBufferInfo xViewProjectionDescriptorBufferInfo;
	memset(&xViewProjectionDescriptorBufferInfo, 0, sizeof(xViewProjectionDescriptorBufferInfo));
	xViewProjectionDescriptorBufferInfo.offset = 0;
	xViewProjectionDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxViewProjectionBuffer);
	xViewProjectionDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxViewProjectionBuffer);

	VkDescriptorBufferInfo xParticleDescriptorBufferInfo;
	memset(&xParticleDescriptorBufferInfo, 0, sizeof(xParticleDescriptorBufferInfo));
	xParticleDescriptorBufferInfo.offset = 0;
	//xParticleDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxParticleBuffer);
	//xParticleDescriptorBufferInfo.range = Buffer_GetSize(pxParticleBuffer); // TODO: sizeof(X) ?

	VkDescriptorImageInfo xSamplerDescriptorImageInfo;
	memset(&xSamplerDescriptorImageInfo, 0, sizeof(xSamplerDescriptorImageInfo));
	xSamplerDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	//xSamplerDescriptorImageInfo.imageView = Image_GetImageView(pxAlbedoImage); // TODO
	xSamplerDescriptorImageInfo.sampler = pxRenderer->axParticleGraphicSampler;

	VkDescriptorImageInfo axSampledImageDescriptorImageInfo[g_nImageAssetsCount];
	memset(axSampledImageDescriptorImageInfo, 0, sizeof(axSampledImageDescriptorImageInfo));

	for (uint32_t i = 0; i < g_nImageAssetsCount; ++i) {
		axSampledImageDescriptorImageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		axSampledImageDescriptorImageInfo[i].imageView = Image_GetImageView(Assets_GetImageByIndex(i));
		axSampledImageDescriptorImageInfo[i].sampler = 0;
	}

	VkWriteDescriptorSet axWriteDescriptorSets[4];

	axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[0].pNext = 0;
	axWriteDescriptorSets[0].dstSet = pxRenderer->xParticleGraphicDescriptorSet;
	axWriteDescriptorSets[0].dstBinding = 0;
	axWriteDescriptorSets[0].dstArrayElement = 0;
	axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[0].descriptorCount = 1;
	axWriteDescriptorSets[0].pImageInfo = 0;
	axWriteDescriptorSets[0].pBufferInfo = &xViewProjectionDescriptorBufferInfo;
	axWriteDescriptorSets[0].pTexelBufferView = 0;

	axWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[1].pNext = 0;
	axWriteDescriptorSets[1].dstSet = pxRenderer->xParticleGraphicDescriptorSet;
	axWriteDescriptorSets[1].dstBinding = 1;
	axWriteDescriptorSets[1].dstArrayElement = 0;
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = 0;
	axWriteDescriptorSets[1].pBufferInfo = &xParticleDescriptorBufferInfo;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	axWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[2].pNext = 0;
	axWriteDescriptorSets[2].dstSet = pxRenderer->xParticleGraphicDescriptorSet;
	axWriteDescriptorSets[2].dstBinding = 2;
	axWriteDescriptorSets[2].dstArrayElement = 0;
	axWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
	axWriteDescriptorSets[2].descriptorCount = 1;
	axWriteDescriptorSets[2].pImageInfo = &xSamplerDescriptorImageInfo;
	axWriteDescriptorSets[2].pBufferInfo = 0;
	axWriteDescriptorSets[2].pTexelBufferView = 0;

	axWriteDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[3].pNext = 0;
	axWriteDescriptorSets[3].dstSet = pxRenderer->xDefaultGraphicDescriptorSet;
	axWriteDescriptorSets[3].dstBinding = 2;
	axWriteDescriptorSets[3].dstArrayElement = 0;
	axWriteDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	axWriteDescriptorSets[3].descriptorCount = g_nImageAssetsCount;
	axWriteDescriptorSets[3].pImageInfo = axSampledImageDescriptorImageInfo;
	axWriteDescriptorSets[3].pBufferInfo = 0;
	axWriteDescriptorSets[3].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}

static void Renderer_UpdateParticleComputeDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorBufferInfo xTimeInfoDescriptorBufferInfo;
	memset(&xTimeInfoDescriptorBufferInfo, 0, sizeof(xTimeInfoDescriptorBufferInfo));
	xTimeInfoDescriptorBufferInfo.offset = 0;
	xTimeInfoDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxTimeInfoBuffer);
	xTimeInfoDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxTimeInfoBuffer);

	VkDescriptorBufferInfo xParticleDescriptorBufferInfo;
	memset(&xParticleDescriptorBufferInfo, 0, sizeof(xParticleDescriptorBufferInfo));
	xParticleDescriptorBufferInfo.offset = 0;
	//xParticleDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxParticleBuffer);
	//xParticleDescriptorBufferInfo.range = Buffer_GetSize(pxParticleBuffer); // TODO: sizeof(X) ?

	VkWriteDescriptorSet axWriteDescriptorSets[3];

	axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[0].pNext = 0;
	axWriteDescriptorSets[0].dstSet = pxRenderer->xParticleComputeDescriptorSet;
	axWriteDescriptorSets[0].dstBinding = 0;
	axWriteDescriptorSets[0].dstArrayElement = 0;
	axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[0].descriptorCount = 1;
	axWriteDescriptorSets[0].pImageInfo = 0;
	axWriteDescriptorSets[0].pBufferInfo = &xTimeInfoDescriptorBufferInfo;
	axWriteDescriptorSets[0].pTexelBufferView = 0;

	axWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[1].pNext = 0;
	axWriteDescriptorSets[1].dstSet = pxRenderer->xParticleComputeDescriptorSet;
	axWriteDescriptorSets[1].dstBinding = 1;
	axWriteDescriptorSets[1].dstArrayElement = 0;
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = 0;
	axWriteDescriptorSets[1].pBufferInfo = &xParticleDescriptorBufferInfo;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	axWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[2].pNext = 0;
	axWriteDescriptorSets[2].dstSet = pxRenderer->xParticleComputeDescriptorSet;
	axWriteDescriptorSets[2].dstBinding = 2;
	axWriteDescriptorSets[2].dstArrayElement = 0;
	axWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axWriteDescriptorSets[2].descriptorCount = 1;
	axWriteDescriptorSets[2].pImageInfo = 0;
	axWriteDescriptorSets[2].pBufferInfo = &xParticleDescriptorBufferInfo;
	axWriteDescriptorSets[2].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}

static void Renderer_CreateDefaultPushConstants(struct xRenderer_t* pxRenderer) {
	// TODO: Split push constant into model and textureIndex
	// TODO: And also rename it to xPerEntityData_t

	pxRenderer->axDefaultGraphicPushConstantRanges[0].offset = 0;
	pxRenderer->axDefaultGraphicPushConstantRanges[0].size = sizeof(xPerObjectData_t);
	pxRenderer->axDefaultGraphicPushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
}

static void Renderer_CreateParticlePushConstants(struct xRenderer_t* pxRenderer) {
	// TODO: Split push constant into model and textureIndex
	// TODO: And also rename it to xPerEntityData_t

	pxRenderer->axParticleGraphicPushConstantRanges[0].offset = 0;
	pxRenderer->axParticleGraphicPushConstantRanges[0].size = sizeof(xPerObjectData_t);
	pxRenderer->axParticleGraphicPushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
}

static void Renderer_CreateDefaultGraphicPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	VkShaderModule xVertModule = Shader_Alloc(pxInstance, "shaders/default.vert.spv");
	VkShaderModule xFragModule = Shader_Alloc(pxInstance, "shaders/default.frag.spv");

	VkVertexInputBindingDescription xVertexInputBindingDescription;
	VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[3];

	Vertex_CreateDescription(&xVertexInputBindingDescription, axVertexInputAttributeDescriptions);

	pxRenderer->pxDefaultGraphicPipeline = GraphicPipeline_Alloc(
		pxInstance,
		pxSwapChain,
		xVertModule,
		xFragModule,
		xVertexInputBindingDescription,
		axVertexInputAttributeDescriptions,
		ARRAY_LENGTH(axVertexInputAttributeDescriptions),
		pxRenderer->xDefaultGraphicDescriptorSetLayout,
		pxRenderer->axDefaultGraphicPushConstantRanges,
		ARRAY_LENGTH(pxRenderer->axDefaultGraphicPushConstantRanges));

	Shader_Free(pxInstance, xVertModule);
	Shader_Free(pxInstance, xFragModule);
}

static void Renderer_CreateParticleGraphicPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	VkShaderModule xVertModule = Shader_Alloc(pxInstance, "shaders/particle.vert.spv");
	VkShaderModule xFragModule = Shader_Alloc(pxInstance, "shaders/particle.frag.spv");

	VkVertexInputBindingDescription xVertexInputBindingDescription;
	VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[3];

	Vertex_CreateDescription(&xVertexInputBindingDescription, axVertexInputAttributeDescriptions);

	pxRenderer->pxParticleGraphicPipeline = GraphicPipeline_Alloc(
		pxInstance,
		pxSwapChain,
		xVertModule,
		xFragModule,
		xVertexInputBindingDescription,
		axVertexInputAttributeDescriptions,
		ARRAY_LENGTH(axVertexInputAttributeDescriptions),
		pxRenderer->xParticleGraphicDescriptorSetLayout,
		pxRenderer->axParticleGraphicPushConstantRanges,
		ARRAY_LENGTH(pxRenderer->axParticleGraphicPushConstantRanges));

	Shader_Free(pxInstance, xVertModule);
	Shader_Free(pxInstance, xFragModule);
}

static void Renderer_CreateParticleComputePipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkShaderModule xCompModule = Shader_Alloc(pxInstance, "shaders/particle.comp.spv");

	pxRenderer->pxParticleComputePipeline = ComputePipeline_Alloc(
		pxInstance,
		xCompModule,
		pxRenderer->xParticleComputeDescriptorSetLayout);

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

static void Renderer_RecordGraphicCommandBuffer(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, uint32_t nImageIndex, struct xList_t* pxEntities) {
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

	{
		vkCmdBindPipeline(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipeline(pxRenderer->pxDefaultGraphicPipeline));

		vkCmdSetViewport(pxRenderer->xGraphicCommandBuffer, 0, 1, &xViewport);
		vkCmdSetScissor(pxRenderer->xGraphicCommandBuffer, 0, 1, &xScissor);

		vkCmdBindDescriptorSets(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipelineLayout(pxRenderer->pxDefaultGraphicPipeline), 0, 1, &pxRenderer->xDefaultGraphicDescriptorSet, 0, 0);

		struct xEntity_t** ppxEntity = List_Begin(pxEntities);
		while (ppxEntity) {
			if (Entity_HasComponents(*ppxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT)) {
				xTransform_t* pxTransform = Entity_GetTransform(*ppxEntity);
				xRenderable_t* pxRenderable = Entity_GetRenderable(*ppxEntity);

				VkBuffer axVertexBuffers[] = { Buffer_GetBuffer(pxRenderable->pxVertexBuffer) };
				uint64_t awOffsets[] = { 0 };

				vkCmdBindVertexBuffers(pxRenderer->xGraphicCommandBuffer, 0, 1, axVertexBuffers, awOffsets);
				vkCmdBindIndexBuffer(pxRenderer->xGraphicCommandBuffer, Buffer_GetBuffer(pxRenderable->pxIndexBuffer), 0, VK_INDEX_TYPE_UINT32);

				static xPerObjectData_t xPerObjectData;
				memset(&xPerObjectData, 0, sizeof(xPerObjectData));

				Matrix_Identity(xPerObjectData.xModel);
				Matrix_SetPosition(xPerObjectData.xModel, pxTransform->xPosition);
				xPerObjectData.nTextureIndex = pxRenderable->nTextureIndex;

				vkCmdPushConstants(pxRenderer->xGraphicCommandBuffer, GraphicPipeline_GetPipelineLayout(pxRenderer->pxDefaultGraphicPipeline), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(xPerObjectData), &xPerObjectData);

				vkCmdDrawIndexed(pxRenderer->xGraphicCommandBuffer, pxRenderable->nIndexCount, 1, 0, 0, 0);
			}

			ppxEntity = List_Next(pxEntities);
		}
	}

	/*
	{
		vkCmdBindPipeline(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipeline(pxRenderer->pxParticleGraphicPipeline));

		vkCmdSetViewport(pxRenderer->xGraphicCommandBuffer, 0, 1, &xViewport);
		vkCmdSetScissor(pxRenderer->xGraphicCommandBuffer, 0, 1, &xScissor);

		vkCmdBindDescriptorSets(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipelineLayout(pxRenderer->pxParticleGraphicPipeline), 0, 1, &pxRenderer->xParticleGraphicDescriptorSet, 0, 0);

		struct xEntity_t** ppxEntity = List_Begin(pxEntities);
		while (ppxEntity) {
			if (Entity_HasComponents(*ppxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT | COMPONENT_PARTICLESYSTEM_BIT)) {
				xTransform_t* pxTransform = Entity_GetTransform(*ppxEntity);
				xRenderable_t* pxRenderable = Entity_GetRenderable(*ppxEntity);
				xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(*ppxEntity);

				VkBuffer axVertexBuffers[] = { Buffer_GetBuffer(pxRenderable->pxVertexBuffer) };
				uint64_t awOffsets[] = { 0 };

				vkCmdBindVertexBuffers(pxRenderer->xGraphicCommandBuffer, 0, 1, axVertexBuffers, awOffsets);
				vkCmdBindIndexBuffer(pxRenderer->xGraphicCommandBuffer, Buffer_GetBuffer(pxRenderable->pxIndexBuffer), 0, VK_INDEX_TYPE_UINT32);

				static xPerObjectData_t xPerObjectData;
				memset(&xPerObjectData, 0, sizeof(xPerObjectData));

				Matrix_Identity(xPerObjectData.xModel);
				Matrix_SetPosition(xPerObjectData.xModel, pxTransform->xPosition);
				xPerObjectData.nTextureIndex = pxRenderable->nTextureIndex;

				vkCmdPushConstants(pxRenderer->xGraphicCommandBuffer, GraphicPipeline_GetPipelineLayout(pxRenderer->pxDefaultGraphicPipeline), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(xPerObjectData), &xPerObjectData);

				vkCmdDrawIndexed(pxRenderer->xGraphicCommandBuffer, pxRenderable->nIndexCount, pxParticleSystem->nParticleCount, 0, 0, 0);
			}

			ppxEntity = List_Next(pxEntities);
		}
	}
	*/

	vkCmdEndRenderPass(pxRenderer->xGraphicCommandBuffer);

	VK_CHECK(vkEndCommandBuffer(pxRenderer->xGraphicCommandBuffer));
}

static void Renderer_RecordComputeCommandBuffer(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xList_t* pxEntities) {
	VkCommandBufferBeginInfo xCommandBufferBeginInfo;
	memset(&xCommandBufferBeginInfo, 0, sizeof(xCommandBufferBeginInfo));
	xCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	xCommandBufferBeginInfo.flags = 0;
	xCommandBufferBeginInfo.pInheritanceInfo = 0;

	VK_CHECK(vkBeginCommandBuffer(pxRenderer->xComputeCommandBuffer, &xCommandBufferBeginInfo));

	{
		Renderer_UpdateParticleComputeDescriptorSets(pxRenderer, pxInstance);

		vkCmdBindPipeline(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipeline(pxRenderer->pxParticleComputePipeline));

		vkCmdBindDescriptorSets(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipelineLayout(pxRenderer->pxParticleComputePipeline), 0, 1, &pxRenderer->xParticleComputeDescriptorSet, 0, 0);

		struct xEntity_t** ppxEntity = List_Begin(pxEntities);
		while (ppxEntity) {
			if (Entity_HasComponents(*ppxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_PARTICLESYSTEM_BIT)) {
				xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(*ppxEntity);

				vkCmdDispatch(pxRenderer->xComputeCommandBuffer, pxParticleSystem->nParticleCount, 1, 1);
			}

			ppxEntity = List_Next(pxEntities);
		}
	}

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

static void Renderer_SubmitGraphic(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xList_t* pxEntities) {
	VK_CHECK(vkWaitForFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xGraphicInFlightFence, VK_TRUE, UINT64_MAX));

	Buffer_Copy(pxRenderer->pxViewProjectionBuffer, &pxRenderer->xViewProjection, sizeof(xViewProjection_t));

	uint32_t nImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(Instance_GetDevice(pxInstance), SwapChain_GetSwapChain(pxSwapChain), UINT64_MAX, pxRenderer->xImageAvailableSemaphore, VK_NULL_HANDLE, &nImageIndex));

	VK_CHECK(vkResetFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xGraphicInFlightFence));

	VK_CHECK(vkResetCommandBuffer(pxRenderer->xGraphicCommandBuffer, 0));

	Renderer_RecordGraphicCommandBuffer(pxRenderer, pxInstance, pxSwapChain, nImageIndex, pxEntities);

	VkSemaphore axWaitSemaphores[] = { /*pxRenderer->xComputeFinishedSemaphore,*/ pxRenderer->xImageAvailableSemaphore }; // TODO
	VkSemaphore axSignalSemaphores[] = { pxRenderer->xGraphicFinishedSemaphore };
	VkPipelineStageFlags axWaitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
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
	xPresentInfo.swapchainCount = ARRAY_LENGTH(axSwapChains);
	xPresentInfo.pSwapchains = axSwapChains;
	xPresentInfo.pImageIndices = &nImageIndex;
	xPresentInfo.waitSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xPresentInfo.pWaitSemaphores = axSignalSemaphores;

	VK_CHECK(vkQueuePresentKHR(Instance_GetPresentQueue(pxInstance), &xPresentInfo));
}

static void Renderer_SubmitCompute(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xList_t* pxEntities) {
	VK_CHECK(vkWaitForFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xComputeInFlightFence, VK_TRUE, UINT64_MAX));

	Buffer_Copy(pxRenderer->pxTimeInfoBuffer, &pxRenderer->xTimeInfo, sizeof(xTimeInfo_t));

	VK_CHECK(vkResetFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xComputeInFlightFence));

	VK_CHECK(vkResetCommandBuffer(pxRenderer->xComputeCommandBuffer, 0));

	Renderer_RecordComputeCommandBuffer(pxRenderer, pxInstance, pxEntities);

	VkSemaphore axSignalSemaphores[] = { pxRenderer->xComputeFinishedSemaphore };

	VkSubmitInfo xSubmitInfo;
	memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
	xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	xSubmitInfo.commandBufferCount = 1;
	xSubmitInfo.pCommandBuffers = &pxRenderer->xComputeCommandBuffer;
	xSubmitInfo.signalSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
	xSubmitInfo.pSignalSemaphores = axSignalSemaphores;

	VK_CHECK(vkQueueSubmit(Instance_GetComputeQueue(pxInstance), 1, &xSubmitInfo, pxRenderer->xComputeInFlightFence));
}

struct xRenderer_t* Renderer_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	struct xRenderer_t* pxRenderer = (struct xRenderer_t*)calloc(1, sizeof(struct xRenderer_t));

	pxRenderer->pxTimeInfoBuffer = UniformBuffer_Alloc(pxInstance, sizeof(xTimeInfo_t));
	pxRenderer->pxViewProjectionBuffer = UniformBuffer_Alloc(pxInstance, sizeof(xViewProjection_t));

	Matrix_Identity(pxRenderer->xViewProjection.xView);
	Matrix_Identity(pxRenderer->xViewProjection.xProjection);

	Renderer_CreateDefaultGraphicDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_CreateParticleGraphicDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_CreateParticleComputeDescriptorSetLayout(pxRenderer, pxInstance);	

	Renderer_CreateDefaultGraphicDescriptorPool(pxRenderer, pxInstance);
	Renderer_CreateParticleGraphicDescriptorPool(pxRenderer, pxInstance);
	Renderer_CreateParticleComputeDescriptorPool(pxRenderer, pxInstance);

	Renderer_CreateDefaultGraphicDescriptorSets(pxRenderer, pxInstance);
	Renderer_CreateParticleGraphicDescriptorSets(pxRenderer, pxInstance);
	Renderer_CreateParticleComputeDescriptorSets(pxRenderer, pxInstance);

	Renderer_CreateDefaultGraphicSampler(pxRenderer, pxInstance);
	Renderer_CreateParticleGraphicSampler(pxRenderer, pxInstance);

	Renderer_CreateDefaultPushConstants(pxRenderer);
	Renderer_CreateParticlePushConstants(pxRenderer);

	Renderer_CreateDefaultGraphicPipeline(pxRenderer, pxInstance, pxSwapChain);
	Renderer_CreateParticleGraphicPipeline(pxRenderer, pxInstance, pxSwapChain);
	Renderer_CreateParticleComputePipeline(pxRenderer, pxInstance);

	Renderer_CreateGraphicCommandBuffer(pxRenderer, pxInstance);
	Renderer_CreateComputeCommandBuffer(pxRenderer, pxInstance);

	Renderer_CreateGraphicSyncObjects(pxRenderer, pxInstance);
	Renderer_CreateComputeSyncObjects(pxRenderer, pxInstance);

	Renderer_UpdateDefaultGraphicDescriptorSets(pxRenderer, pxInstance);
	//Renderer_UpdateParticleGraphicDescriptorSets(pxRenderer, pxInstance);

	return pxRenderer;
}

void Renderer_Free(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyFence(Instance_GetDevice(pxInstance), pxRenderer->xComputeInFlightFence, 0);
	vkDestroyFence(Instance_GetDevice(pxInstance), pxRenderer->xGraphicInFlightFence, 0);

	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xComputeFinishedSemaphore, 0);
	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xGraphicFinishedSemaphore, 0);
	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xImageAvailableSemaphore, 0);

	vkDestroySampler(Instance_GetDevice(pxInstance), pxRenderer->axDefaultGraphicSampler, 0);
	vkDestroySampler(Instance_GetDevice(pxInstance), pxRenderer->axParticleGraphicSampler, 0);

	ComputePipeline_Free(pxRenderer->pxParticleComputePipeline, pxInstance);
	GraphicPipeline_Free(pxRenderer->pxParticleGraphicPipeline, pxInstance);
	GraphicPipeline_Free(pxRenderer->pxDefaultGraphicPipeline, pxInstance);

	Buffer_Free(pxRenderer->pxViewProjectionBuffer, pxInstance);
	Buffer_Free(pxRenderer->pxTimeInfoBuffer, pxInstance);

	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xParticleComputeDescriptorPool, 0);
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xParticleGraphicDescriptorPool, 0);
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xDefaultGraphicDescriptorPool, 0);

	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xParticleComputeDescriptorSetLayout, 0);
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xParticleGraphicDescriptorSetLayout, 0);
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xDefaultGraphicDescriptorSetLayout, 0);

	free(pxRenderer);
}

xTimeInfo_t* Renderer_GetTimeInfo(struct xRenderer_t* pxRenderer) {
	return &pxRenderer->xTimeInfo;
}

xViewProjection_t* Renderer_GetViewProjection(struct xRenderer_t* pxRenderer) {
	return &pxRenderer->xViewProjection;
}

void Renderer_Draw(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xList_t* pxEntities) {
	//Renderer_SubmitCompute(pxRenderer, pxInstance, pxEntities);
	Renderer_SubmitGraphic(pxRenderer, pxInstance, pxSwapChain, pxEntities);
}
