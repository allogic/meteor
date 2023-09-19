#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common/macros.h>

#include <container/list.h>
#include <container/queue.h>
#include <container/vector.h>

#include <math/matrix.h>

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

#include <game/renderer.h>
#include <game/component.h>
#include <game/entity.h>

#define INITIAL_DEFAULT_GRAPHIC_POOL_COUNT 1024
#define INITIAL_PARTICLE_GRAPHIC_POOL_COUNT 1024
#define INITIAL_PARTICLE_COMPUTE_POOL_COUNT 1024
#define INITIAL_PIXEL_COMPUTE_POOL_COUNT 1024

struct xRenderer_t {
	xTimeInfo_t xTimeInfo;
	xViewProjection_t xViewProjection;

	struct xBuffer_t* pxTimeInfoBuffer;
	struct xBuffer_t* pxViewProjectionBuffer;

	VkDescriptorPool xDefaultGraphicDescriptorPool;
	VkDescriptorPool xParticleGraphicDescriptorPool;
	VkDescriptorPool xParticleComputeDescriptorPool;
	VkDescriptorPool xPixelComputeDescriptorPool;

	VkDescriptorSetLayout xDefaultGraphicDescriptorSetLayout;
	VkDescriptorSetLayout xParticleGraphicDescriptorSetLayout;
	VkDescriptorSetLayout xParticleComputeDescriptorSetLayout;
	VkDescriptorSetLayout xPixelComputeDescriptorSetLayout;

	struct xVector_t* pxDefaultGraphicDescriptorSets;
	struct xVector_t* pxParticleGraphicDescriptorSets;
	struct xVector_t* pxParticleComputeDescriptorSets;
	struct xVector_t* pxPixelComputeDescriptorSets;

	VkPushConstantRange axDefaultGraphicPushConstantRanges[1];
	VkPushConstantRange axParticleGraphicPushConstantRanges[1];

	struct xGraphicPipeline_t* pxDefaultGraphicPipeline;
	struct xGraphicPipeline_t* pxParticleGraphicPipeline;
	struct xComputePipeline_t* pxParticleComputePipeline;
	struct xComputePipeline_t* pxPixelComputePipeline;

	VkCommandBuffer xGraphicCommandBuffer;
	VkCommandBuffer xComputeCommandBuffer;

	VkSemaphore xImageAvailableSemaphore;
	VkSemaphore xGraphicFinishedSemaphore;
	VkSemaphore xComputeFinishedSemaphore;

	VkFence xGraphicInFlightFence;
	VkFence xComputeInFlightFence;
};

static void Renderer_AllocDefaultGraphicDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
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
	xDescriptorPoolCreateInfo.maxSets = INITIAL_DEFAULT_GRAPHIC_POOL_COUNT;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xDefaultGraphicDescriptorPool));
}
static void Renderer_AllocParticleGraphicDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorPoolSize axDescriptorPoolSizes[3];

	axDescriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorPoolSizes[0].descriptorCount = 1;

	axDescriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axDescriptorPoolSizes[1].descriptorCount = 1;

	axDescriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	axDescriptorPoolSizes[2].descriptorCount = 1;

	VkDescriptorPoolCreateInfo xDescriptorPoolCreateInfo;
	memset(&xDescriptorPoolCreateInfo, 0, sizeof(xDescriptorPoolCreateInfo));
	xDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	xDescriptorPoolCreateInfo.poolSizeCount = ARRAY_LENGTH(axDescriptorPoolSizes);
	xDescriptorPoolCreateInfo.pPoolSizes = axDescriptorPoolSizes;
	xDescriptorPoolCreateInfo.maxSets = INITIAL_PARTICLE_GRAPHIC_POOL_COUNT;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xParticleGraphicDescriptorPool));
}
static void Renderer_AllocParticleComputeDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
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
	xDescriptorPoolCreateInfo.maxSets = INITIAL_PARTICLE_COMPUTE_POOL_COUNT;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xParticleComputeDescriptorPool));
}
static void Renderer_AllocPixelComputeDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorPoolSize axDescriptorPoolSizes[2];

	axDescriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorPoolSizes[0].descriptorCount = 1;

	axDescriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorPoolSizes[1].descriptorCount = 5;

	VkDescriptorPoolCreateInfo xDescriptorPoolCreateInfo;
	memset(&xDescriptorPoolCreateInfo, 0, sizeof(xDescriptorPoolCreateInfo));
	xDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	xDescriptorPoolCreateInfo.poolSizeCount = ARRAY_LENGTH(axDescriptorPoolSizes);
	xDescriptorPoolCreateInfo.pPoolSizes = axDescriptorPoolSizes;
	xDescriptorPoolCreateInfo.maxSets = INITIAL_PIXEL_COMPUTE_POOL_COUNT;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xPixelComputeDescriptorPool));
}

static void Renderer_FreeDefaultGraphicDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xDefaultGraphicDescriptorPool, 0);
}
static void Renderer_FreeParticleGraphicDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xParticleGraphicDescriptorPool, 0);
}
static void Renderer_FreeParticleComputeDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xParticleComputeDescriptorPool, 0);
}
static void Renderer_FreePixelComputeDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xPixelComputeDescriptorPool, 0);
}

static void Renderer_AllocDefaultGraphicDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
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

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xDefaultGraphicDescriptorSetLayout));
}
static void Renderer_AllocParticleGraphicDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[3];

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
	axDescriptorSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	axDescriptorSetLayoutBindings[2].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = ARRAY_LENGTH(axDescriptorSetLayoutBindings);
	xDescriptorSetLayoutCreateInfo.pBindings = axDescriptorSetLayoutBindings;

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xParticleGraphicDescriptorSetLayout));
}
static void Renderer_AllocParticleComputeDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
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
static void Renderer_AllocPixelComputeDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[6];

	axDescriptorSetLayoutBindings[0].binding = 0;
	axDescriptorSetLayoutBindings[0].descriptorCount = 1;
	axDescriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorSetLayoutBindings[0].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[1].binding = 1;
	axDescriptorSetLayoutBindings[1].descriptorCount = 1;
	axDescriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[1].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[2].binding = 2;
	axDescriptorSetLayoutBindings[2].descriptorCount = 1;
	axDescriptorSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[2].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[3].binding = 3;
	axDescriptorSetLayoutBindings[3].descriptorCount = 1;
	axDescriptorSetLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[3].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[4].binding = 4;
	axDescriptorSetLayoutBindings[4].descriptorCount = 1;
	axDescriptorSetLayoutBindings[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[4].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[4].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[5].binding = 5;
	axDescriptorSetLayoutBindings[5].descriptorCount = 1;
	axDescriptorSetLayoutBindings[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[5].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[5].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = ARRAY_LENGTH(axDescriptorSetLayoutBindings);
	xDescriptorSetLayoutCreateInfo.pBindings = axDescriptorSetLayoutBindings;

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xPixelComputeDescriptorSetLayout));
}

static void Renderer_FreeDefaultGraphicDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xDefaultGraphicDescriptorSetLayout, 0);
}
static void Renderer_FreeParticleGraphicDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xParticleGraphicDescriptorSetLayout, 0);
}
static void Renderer_FreeParticleComputeDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xParticleComputeDescriptorSetLayout, 0);
}
static void Renderer_FreePixelComputeDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xPixelComputeDescriptorSetLayout, 0);
}

static void Renderer_CreateDefaultGraphicDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetCount) {
	VkDescriptorSetLayout axDescriptorSetLayouts[INITIAL_DEFAULT_GRAPHIC_POOL_COUNT];

	// TODO: Cache this..
	for (uint32_t i = 0; i < INITIAL_DEFAULT_GRAPHIC_POOL_COUNT; ++i) {
		axDescriptorSetLayouts[i] = pxRenderer->xDefaultGraphicDescriptorSetLayout;
	}

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xDefaultGraphicDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = nDescriptorSetCount;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, Vector_Data(pxRenderer->pxDefaultGraphicDescriptorSets)));
}
static void Renderer_CreateParticleGraphicDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetCount) {
	VkDescriptorSetLayout axDescriptorSetLayouts[INITIAL_PARTICLE_GRAPHIC_POOL_COUNT];

	// TODO: Cache this..
	for (uint32_t i = 0; i < INITIAL_PARTICLE_GRAPHIC_POOL_COUNT; ++i) {
		axDescriptorSetLayouts[i] = pxRenderer->xParticleGraphicDescriptorSetLayout;
	}

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xParticleGraphicDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = nDescriptorSetCount;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, Vector_Data(pxRenderer->pxParticleGraphicDescriptorSets)));
}
static void Renderer_CreateParticleComputeDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetCount) {
	VkDescriptorSetLayout axDescriptorSetLayouts[INITIAL_PARTICLE_COMPUTE_POOL_COUNT];

	// TODO: Cache this..
	for (uint32_t i = 0; i < INITIAL_PARTICLE_COMPUTE_POOL_COUNT; ++i) {
		axDescriptorSetLayouts[i] = pxRenderer->xParticleComputeDescriptorSetLayout;
	}

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xParticleComputeDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = nDescriptorSetCount;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, Vector_Data(pxRenderer->pxParticleComputeDescriptorSets)));
}
static void Renderer_CreatePixelComputeDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetCount) {
	VkDescriptorSetLayout axDescriptorSetLayouts[INITIAL_PIXEL_COMPUTE_POOL_COUNT];

	// TODO: Cache this..
	for (uint32_t i = 0; i < INITIAL_PIXEL_COMPUTE_POOL_COUNT; ++i) {
		axDescriptorSetLayouts[i] = pxRenderer->xPixelComputeDescriptorSetLayout;
	}

	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xPixelComputeDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = nDescriptorSetCount;
	xDescriptorSetAllocateInfo.pSetLayouts = axDescriptorSetLayouts;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, Vector_Data(pxRenderer->pxPixelComputeDescriptorSets)));
}

static void Renderer_UpdateDefaultGraphicDescriptorSet(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetIndex, xRenderable_t* pxRenderable) {
	VkDescriptorBufferInfo xViewProjectionDescriptorBufferInfo;
	memset(&xViewProjectionDescriptorBufferInfo, 0, sizeof(xViewProjectionDescriptorBufferInfo));
	xViewProjectionDescriptorBufferInfo.offset = 0;
	xViewProjectionDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxViewProjectionBuffer);
	xViewProjectionDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxViewProjectionBuffer);

	VkDescriptorImageInfo xCombinedImageSamplerDescriptorImageInfo;
	memset(&xCombinedImageSamplerDescriptorImageInfo, 0, sizeof(xCombinedImageSamplerDescriptorImageInfo));
	xCombinedImageSamplerDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	xCombinedImageSamplerDescriptorImageInfo.imageView = Image_GetImageView(pxRenderable->pxAlbedoImage);
	xCombinedImageSamplerDescriptorImageInfo.sampler = Image_GetSampler(pxRenderable->pxAlbedoImage);

	VkWriteDescriptorSet axWriteDescriptorSets[2];

	axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[0].pNext = 0;
	axWriteDescriptorSets[0].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxDefaultGraphicDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[0].dstBinding = 0;
	axWriteDescriptorSets[0].dstArrayElement = 0;
	axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[0].descriptorCount = 1;
	axWriteDescriptorSets[0].pImageInfo = 0;
	axWriteDescriptorSets[0].pBufferInfo = &xViewProjectionDescriptorBufferInfo;
	axWriteDescriptorSets[0].pTexelBufferView = 0;

	axWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[1].pNext = 0;
	axWriteDescriptorSets[1].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxDefaultGraphicDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[1].dstBinding = 1;
	axWriteDescriptorSets[1].dstArrayElement = 0;
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = &xCombinedImageSamplerDescriptorImageInfo;
	axWriteDescriptorSets[1].pBufferInfo = 0;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}
static void Renderer_UpdateParticleGraphicDescriptorSet(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetIndex, xRenderable_t* pxRenderable, xParticleSystem_t* pxParticleSystem) {
	VkDescriptorBufferInfo xViewProjectionDescriptorBufferInfo;
	memset(&xViewProjectionDescriptorBufferInfo, 0, sizeof(xViewProjectionDescriptorBufferInfo));
	xViewProjectionDescriptorBufferInfo.offset = 0;
	xViewProjectionDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxViewProjectionBuffer);
	xViewProjectionDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxViewProjectionBuffer);

	VkDescriptorBufferInfo xParticleDescriptorBufferInfo;
	memset(&xParticleDescriptorBufferInfo, 0, sizeof(xParticleDescriptorBufferInfo));
	xParticleDescriptorBufferInfo.offset = 0;
	xParticleDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxParticleSystem->pxParticleBuffer);
	xParticleDescriptorBufferInfo.range = Buffer_GetSize(pxParticleSystem->pxParticleBuffer);

	VkDescriptorImageInfo xCombinedImageSamplerDescriptorImageInfo;
	memset(&xCombinedImageSamplerDescriptorImageInfo, 0, sizeof(xCombinedImageSamplerDescriptorImageInfo));
	xCombinedImageSamplerDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	xCombinedImageSamplerDescriptorImageInfo.imageView = Image_GetImageView(pxRenderable->pxAlbedoImage);
	xCombinedImageSamplerDescriptorImageInfo.sampler = Image_GetSampler(pxRenderable->pxAlbedoImage);

	VkWriteDescriptorSet axWriteDescriptorSets[3];

	axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[0].pNext = 0;
	axWriteDescriptorSets[0].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxParticleGraphicDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[0].dstBinding = 0;
	axWriteDescriptorSets[0].dstArrayElement = 0;
	axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[0].descriptorCount = 1;
	axWriteDescriptorSets[0].pImageInfo = 0;
	axWriteDescriptorSets[0].pBufferInfo = &xViewProjectionDescriptorBufferInfo;
	axWriteDescriptorSets[0].pTexelBufferView = 0;

	axWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[1].pNext = 0;
	axWriteDescriptorSets[1].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxParticleGraphicDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[1].dstBinding = 1;
	axWriteDescriptorSets[1].dstArrayElement = 0;
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = 0;
	axWriteDescriptorSets[1].pBufferInfo = &xParticleDescriptorBufferInfo;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	axWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[2].pNext = 0;
	axWriteDescriptorSets[2].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxParticleGraphicDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[2].dstBinding = 2;
	axWriteDescriptorSets[2].dstArrayElement = 0;
	axWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	axWriteDescriptorSets[2].descriptorCount = 1;
	axWriteDescriptorSets[2].pImageInfo = &xCombinedImageSamplerDescriptorImageInfo;
	axWriteDescriptorSets[2].pBufferInfo = 0;
	axWriteDescriptorSets[2].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}
static void Renderer_UpdateParticleComputeDescriptorSet(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetIndex, xParticleSystem_t* pxParticleSystem) {
	VkDescriptorBufferInfo xTimeInfoDescriptorBufferInfo;
	memset(&xTimeInfoDescriptorBufferInfo, 0, sizeof(xTimeInfoDescriptorBufferInfo));
	xTimeInfoDescriptorBufferInfo.offset = 0;
	xTimeInfoDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxTimeInfoBuffer);
	xTimeInfoDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxTimeInfoBuffer);

	VkDescriptorBufferInfo xParticleDescriptorBufferInfo;
	memset(&xParticleDescriptorBufferInfo, 0, sizeof(xParticleDescriptorBufferInfo));
	xParticleDescriptorBufferInfo.offset = 0;
	xParticleDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxParticleSystem->pxParticleBuffer);
	xParticleDescriptorBufferInfo.range = Buffer_GetSize(pxParticleSystem->pxParticleBuffer);

	VkWriteDescriptorSet axWriteDescriptorSets[3];

	axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[0].pNext = 0;
	axWriteDescriptorSets[0].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxParticleComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[0].dstBinding = 0;
	axWriteDescriptorSets[0].dstArrayElement = 0;
	axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[0].descriptorCount = 1;
	axWriteDescriptorSets[0].pImageInfo = 0;
	axWriteDescriptorSets[0].pBufferInfo = &xTimeInfoDescriptorBufferInfo;
	axWriteDescriptorSets[0].pTexelBufferView = 0;

	axWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[1].pNext = 0;
	axWriteDescriptorSets[1].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxParticleComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[1].dstBinding = 1;
	axWriteDescriptorSets[1].dstArrayElement = 0;
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = 0;
	axWriteDescriptorSets[1].pBufferInfo = &xParticleDescriptorBufferInfo;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	axWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[2].pNext = 0;
	axWriteDescriptorSets[2].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxParticleComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[2].dstBinding = 2;
	axWriteDescriptorSets[2].dstArrayElement = 0;
	axWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axWriteDescriptorSets[2].descriptorCount = 1;
	axWriteDescriptorSets[2].pImageInfo = 0;
	axWriteDescriptorSets[2].pBufferInfo = &xParticleDescriptorBufferInfo;
	axWriteDescriptorSets[2].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}
static void Renderer_UpdatePixelComputeDescriptorSet(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetIndex, xPixelSystem_t* pxPixelSystem) {
	VkDescriptorBufferInfo xTimeInfoDescriptorBufferInfo;
	memset(&xTimeInfoDescriptorBufferInfo, 0, sizeof(xTimeInfoDescriptorBufferInfo));
	xTimeInfoDescriptorBufferInfo.offset = 0;
	xTimeInfoDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxTimeInfoBuffer);
	xTimeInfoDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxTimeInfoBuffer);

	VkDescriptorImageInfo xNorthDescriptorImageInfo;
	memset(&xNorthDescriptorImageInfo, 0, sizeof(xNorthDescriptorImageInfo));
	xNorthDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xNorthDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxImageN);
	xNorthDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxImageN);

	VkDescriptorImageInfo xSouthDescriptorImageInfo;
	memset(&xSouthDescriptorImageInfo, 0, sizeof(xSouthDescriptorImageInfo));
	xSouthDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xSouthDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxImageS);
	xSouthDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxImageS);

	VkDescriptorImageInfo xWestDescriptorImageInfo;
	memset(&xWestDescriptorImageInfo, 0, sizeof(xWestDescriptorImageInfo));
	xWestDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xWestDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxImageW);
	xWestDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxImageW);

	VkDescriptorImageInfo xEastDescriptorImageInfo;
	memset(&xEastDescriptorImageInfo, 0, sizeof(xEastDescriptorImageInfo));
	xEastDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xEastDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxImageE);
	xEastDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxImageE);

	VkDescriptorImageInfo xDescriptorImageInfo;
	memset(&xDescriptorImageInfo, 0, sizeof(xDescriptorImageInfo));
	xDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxImage);
	xDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxImage);

	VkWriteDescriptorSet axWriteDescriptorSets[6];

	axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[0].pNext = 0;
	axWriteDescriptorSets[0].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[0].dstBinding = 0;
	axWriteDescriptorSets[0].dstArrayElement = 0;
	axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[0].descriptorCount = 1;
	axWriteDescriptorSets[0].pImageInfo = 0;
	axWriteDescriptorSets[0].pBufferInfo = &xTimeInfoDescriptorBufferInfo;
	axWriteDescriptorSets[0].pTexelBufferView = 0;

	axWriteDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[1].pNext = 0;
	axWriteDescriptorSets[1].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[1].dstBinding = 1;
	axWriteDescriptorSets[1].dstArrayElement = 0;
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = &xNorthDescriptorImageInfo;
	axWriteDescriptorSets[1].pBufferInfo = 0;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	axWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[2].pNext = 0;
	axWriteDescriptorSets[2].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[2].dstBinding = 2;
	axWriteDescriptorSets[2].dstArrayElement = 0;
	axWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[2].descriptorCount = 1;
	axWriteDescriptorSets[2].pImageInfo = &xSouthDescriptorImageInfo;
	axWriteDescriptorSets[2].pBufferInfo = 0;
	axWriteDescriptorSets[2].pTexelBufferView = 0;

	axWriteDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[3].pNext = 0;
	axWriteDescriptorSets[3].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[3].dstBinding = 3;
	axWriteDescriptorSets[3].dstArrayElement = 0;
	axWriteDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[3].descriptorCount = 1;
	axWriteDescriptorSets[3].pImageInfo = &xWestDescriptorImageInfo;
	axWriteDescriptorSets[3].pBufferInfo = 0;
	axWriteDescriptorSets[3].pTexelBufferView = 0;

	axWriteDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[4].pNext = 0;
	axWriteDescriptorSets[4].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[4].dstBinding = 4;
	axWriteDescriptorSets[4].dstArrayElement = 0;
	axWriteDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[4].descriptorCount = 1;
	axWriteDescriptorSets[4].pImageInfo = &xEastDescriptorImageInfo;
	axWriteDescriptorSets[4].pBufferInfo = 0;
	axWriteDescriptorSets[4].pTexelBufferView = 0;

	axWriteDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[5].pNext = 0;
	axWriteDescriptorSets[5].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[5].dstBinding = 5;
	axWriteDescriptorSets[5].dstArrayElement = 0;
	axWriteDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[5].descriptorCount = 1;
	axWriteDescriptorSets[5].pImageInfo = &xDescriptorImageInfo;
	axWriteDescriptorSets[5].pBufferInfo = 0;
	axWriteDescriptorSets[5].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}

static void Renderer_AllocDefaultPushConstants(struct xRenderer_t* pxRenderer) {
	pxRenderer->axDefaultGraphicPushConstantRanges[0].offset = 0;
	pxRenderer->axDefaultGraphicPushConstantRanges[0].size = sizeof(xPerEntityData_t);
	pxRenderer->axDefaultGraphicPushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
}
static void Renderer_AllocParticlePushConstants(struct xRenderer_t* pxRenderer) {
	pxRenderer->axParticleGraphicPushConstantRanges[0].offset = 0;
	pxRenderer->axParticleGraphicPushConstantRanges[0].size = sizeof(xPerEntityData_t);
	pxRenderer->axParticleGraphicPushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
}

static void Renderer_AllocDefaultGraphicPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	VkShaderModule xVertModule = Shader_Alloc(pxInstance, "shaders/default.vert.spv");
	VkShaderModule xFragModule = Shader_Alloc(pxInstance, "shaders/default.frag.spv");

	VkVertexInputBindingDescription xVertexInputBindingDescription;
	VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[3];

	Vertex_AllocDescription(&xVertexInputBindingDescription, axVertexInputAttributeDescriptions);

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
static void Renderer_AllocParticleGraphicPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	VkShaderModule xVertModule = Shader_Alloc(pxInstance, "shaders/particle.vert.spv");
	VkShaderModule xFragModule = Shader_Alloc(pxInstance, "shaders/particle.frag.spv");

	VkVertexInputBindingDescription xVertexInputBindingDescription;
	VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[3];

	Vertex_AllocDescription(&xVertexInputBindingDescription, axVertexInputAttributeDescriptions);

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
static void Renderer_AllocParticleComputePipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkShaderModule xCompModule = Shader_Alloc(pxInstance, "shaders/particle.comp.spv");

	pxRenderer->pxParticleComputePipeline = ComputePipeline_Alloc(
		pxInstance,
		xCompModule,
		pxRenderer->xParticleComputeDescriptorSetLayout);

	Shader_Free(pxInstance, xCompModule);
}
static void Renderer_AllocPixelComputePipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkShaderModule xCompModule = Shader_Alloc(pxInstance, "shaders/pixel.comp.spv");

	pxRenderer->pxPixelComputePipeline = ComputePipeline_Alloc(
		pxInstance,
		xCompModule,
		pxRenderer->xPixelComputeDescriptorSetLayout);

	Shader_Free(pxInstance, xCompModule);
}

static void Renderer_FreeDefaultGraphicPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	GraphicPipeline_Free(pxRenderer->pxDefaultGraphicPipeline, pxInstance);
}
static void Renderer_FreeParticleGraphicPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	GraphicPipeline_Free(pxRenderer->pxParticleGraphicPipeline, pxInstance);
}
static void Renderer_FreeParticleComputePipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	ComputePipeline_Free(pxRenderer->pxParticleComputePipeline, pxInstance);
}
static void Renderer_FreePixelComputePipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	ComputePipeline_Free(pxRenderer->pxPixelComputePipeline, pxInstance);
}

static void Renderer_AllocGraphicCommandBuffer(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkCommandBufferAllocateInfo xCommandBufferAllocCreateInfo;
	memset(&xCommandBufferAllocCreateInfo, 0, sizeof(xCommandBufferAllocCreateInfo));
	xCommandBufferAllocCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xCommandBufferAllocCreateInfo.commandPool = Instance_GetCommandPool(pxInstance);
	xCommandBufferAllocCreateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xCommandBufferAllocCreateInfo.commandBufferCount = 1;

	VK_CHECK(vkAllocateCommandBuffers(Instance_GetDevice(pxInstance), &xCommandBufferAllocCreateInfo, &pxRenderer->xGraphicCommandBuffer));
}
static void Renderer_AllocComputeCommandBuffer(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
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

		vkCmdBindDescriptorSets(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipelineLayout(pxRenderer->pxDefaultGraphicPipeline), 0, 1, Vector_Data(pxRenderer->pxDefaultGraphicDescriptorSets), 0, 0);

		void* pIter = List_Begin(pxEntities);
		while (pIter) {
			struct xEntity_t** ppxEntity = List_Value(pIter);

			if (Entity_HasComponents(*ppxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT)) {
				xTransform_t* pxTransform = Entity_GetTransform(*ppxEntity);
				xRenderable_t* pxRenderable = Entity_GetRenderable(*ppxEntity);

				Image_LayoutTransition(pxRenderable->pxAlbedoImage, pxInstance, pxRenderer->xGraphicCommandBuffer, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

				VkBuffer axVertexBuffers[] = { Buffer_GetBuffer(pxRenderable->pxVertexBuffer) };
				uint64_t awOffsets[] = { 0 };

				vkCmdBindVertexBuffers(pxRenderer->xGraphicCommandBuffer, 0, 1, axVertexBuffers, awOffsets);
				vkCmdBindIndexBuffer(pxRenderer->xGraphicCommandBuffer, Buffer_GetBuffer(pxRenderable->pxIndexBuffer), 0, VK_INDEX_TYPE_UINT32);

				static xPerEntityData_t xPerEntityData;
				memset(&xPerEntityData, 0, sizeof(xPerEntityData));

				// TODO: Maybe also do this on the GPU (compute shader and linear transformation buffer)
				Matrix_Identity(xPerEntityData.xModel);
				Matrix_SetPosition(xPerEntityData.xModel, pxTransform->xPosition);
				//Matrix_RotateX(xPerEntityData.xModel, pxTransform->xRotation[0]);
				//Matrix_RotateY(xPerEntityData.xModel, pxTransform->xRotation[0]);
				//Matrix_RotateZ(xPerEntityData.xModel, pxTransform->xRotation[0]);
				Matrix_SetScale(xPerEntityData.xModel, pxTransform->xScale);

				vkCmdPushConstants(pxRenderer->xGraphicCommandBuffer, GraphicPipeline_GetPipelineLayout(pxRenderer->pxDefaultGraphicPipeline), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(xPerEntityData), &xPerEntityData);

				vkCmdDrawIndexed(pxRenderer->xGraphicCommandBuffer, pxRenderable->nIndexCount, 1, 0, 0, 0);
			}

			pIter = List_Next(pIter);
		}
	}

	{
		vkCmdBindPipeline(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipeline(pxRenderer->pxParticleGraphicPipeline));

		vkCmdSetViewport(pxRenderer->xGraphicCommandBuffer, 0, 1, &xViewport);
		vkCmdSetScissor(pxRenderer->xGraphicCommandBuffer, 0, 1, &xScissor);

		vkCmdBindDescriptorSets(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipelineLayout(pxRenderer->pxParticleGraphicPipeline), 0, 1, Vector_Data(pxRenderer->pxParticleGraphicDescriptorSets), 0, 0);

		void* pIter = List_Begin(pxEntities);
		while (pIter) {
			struct xEntity_t** ppxEntity = List_Value(pIter);

			if (Entity_HasComponents(*ppxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT | COMPONENT_PARTICLESYSTEM_BIT)) {
				xTransform_t* pxTransform = Entity_GetTransform(*ppxEntity);
				xRenderable_t* pxRenderable = Entity_GetRenderable(*ppxEntity);
				xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(*ppxEntity);

				Image_LayoutTransition(pxRenderable->pxAlbedoImage, pxInstance, pxRenderer->xGraphicCommandBuffer, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

				VkBuffer axVertexBuffers[] = { Buffer_GetBuffer(pxRenderable->pxVertexBuffer) };
				uint64_t awOffsets[] = { 0 };

				vkCmdBindVertexBuffers(pxRenderer->xGraphicCommandBuffer, 0, 1, axVertexBuffers, awOffsets);
				vkCmdBindIndexBuffer(pxRenderer->xGraphicCommandBuffer, Buffer_GetBuffer(pxRenderable->pxIndexBuffer), 0, VK_INDEX_TYPE_UINT32);

				static xPerEntityData_t xPerEntityData;
				memset(&xPerEntityData, 0, sizeof(xPerEntityData));

				// TODO: Maybe also do this on the GPU (compute shader and linear transformation buffer)
				Matrix_Identity(xPerEntityData.xModel);
				Matrix_SetPosition(xPerEntityData.xModel, pxTransform->xPosition);
				//Matrix_RotateX(xPerEntityData.xModel, pxTransform->xRotation[0]);
				//Matrix_RotateY(xPerEntityData.xModel, pxTransform->xRotation[0]);
				//Matrix_RotateZ(xPerEntityData.xModel, pxTransform->xRotation[0]);
				Matrix_SetScale(xPerEntityData.xModel, pxTransform->xScale);

				vkCmdPushConstants(pxRenderer->xGraphicCommandBuffer, GraphicPipeline_GetPipelineLayout(pxRenderer->pxDefaultGraphicPipeline), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(xPerEntityData), &xPerEntityData);

				vkCmdDrawIndexed(pxRenderer->xGraphicCommandBuffer, pxRenderable->nIndexCount, pxParticleSystem->nParticleCount, 0, 0, 0);
			}

			pIter = List_Next(pIter);
		}
	}

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
		vkCmdBindPipeline(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipeline(pxRenderer->pxParticleComputePipeline));

		vkCmdBindDescriptorSets(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipelineLayout(pxRenderer->pxParticleComputePipeline), 0, 1, Vector_Data(pxRenderer->pxParticleComputeDescriptorSets), 0, 0);

		void* pIter = List_Begin(pxEntities);
		while (pIter) {
			struct xEntity_t** ppxEntity = List_Value(pIter);

			if (Entity_HasComponents(*ppxEntity, COMPONENT_PARTICLESYSTEM_BIT)) {
				xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(*ppxEntity);

				vkCmdDispatch(pxRenderer->xComputeCommandBuffer, pxParticleSystem->nParticleCount / 256, 1, 1);
			}

			pIter = List_Next(pIter);
		}
	}

	{
		vkCmdBindPipeline(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipeline(pxRenderer->pxPixelComputePipeline));

		vkCmdBindDescriptorSets(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipelineLayout(pxRenderer->pxPixelComputePipeline), 0, 1, Vector_Data(pxRenderer->pxPixelComputeDescriptorSets), 0, 0);

		void* pIter = List_Begin(pxEntities);
		while (pIter) {
			struct xEntity_t** ppxEntity = List_Value(pIter);

			if (Entity_HasComponents(*ppxEntity, COMPONENT_PIXELSYSTEM_BIT)) {
				xPixelSystem_t* pxPixelSystem = Entity_GetPixelSystem(*ppxEntity);

				vkCmdDispatch(pxRenderer->xComputeCommandBuffer, pxPixelSystem->nImageWidth / 256, pxPixelSystem->nImageHeight / 256, 1);
			}

			pIter = List_Next(pIter);
		}
	}

	VK_CHECK(vkEndCommandBuffer(pxRenderer->xComputeCommandBuffer));
}

static void Renderer_AllocGraphicSynchronizationObjects(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkSemaphoreCreateInfo xSemaphoreCreateInfo;
	memset(&xSemaphoreCreateInfo, 0, sizeof(xSemaphoreCreateInfo));
	xSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo xFenceCreateInfo;
	memset(&xFenceCreateInfo, 0, sizeof(xFenceCreateInfo));
	xFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	xFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->xGraphicFinishedSemaphore));
	VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->xImageAvailableSemaphore));

	VK_CHECK(vkCreateFence(Instance_GetDevice(pxInstance), &xFenceCreateInfo, 0, &pxRenderer->xGraphicInFlightFence));
}
static void Renderer_AllocComputeSynchronizationObjects(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
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

static void Renderer_FreeGraphicSynchronizationObjects(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xGraphicFinishedSemaphore, 0);
	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xImageAvailableSemaphore, 0);

	vkDestroyFence(Instance_GetDevice(pxInstance), pxRenderer->xGraphicInFlightFence, 0);
}
static void Renderer_FreeComputeSynchronizationObjects(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xComputeFinishedSemaphore, 0);

	vkDestroyFence(Instance_GetDevice(pxInstance), pxRenderer->xComputeInFlightFence, 0);
}

static void Renderer_SubmitGraphic(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xList_t* pxEntities) {
	VK_CHECK(vkWaitForFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xGraphicInFlightFence, VK_TRUE, UINT64_MAX));

	Buffer_SetTo(pxRenderer->pxViewProjectionBuffer, &pxRenderer->xViewProjection, sizeof(xViewProjection_t));

	uint32_t nImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(Instance_GetDevice(pxInstance), SwapChain_GetSwapChain(pxSwapChain), UINT64_MAX, pxRenderer->xImageAvailableSemaphore, VK_NULL_HANDLE, &nImageIndex));

	VK_CHECK(vkResetFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xGraphicInFlightFence));

	VK_CHECK(vkResetCommandBuffer(pxRenderer->xGraphicCommandBuffer, 0));

	Renderer_RecordGraphicCommandBuffer(pxRenderer, pxInstance, pxSwapChain, nImageIndex, pxEntities);

	VkSemaphore axWaitSemaphores[] = { pxRenderer->xComputeFinishedSemaphore, pxRenderer->xImageAvailableSemaphore };
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

	Buffer_SetTo(pxRenderer->pxTimeInfoBuffer, &pxRenderer->xTimeInfo, sizeof(xTimeInfo_t));

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

	pxRenderer->pxDefaultGraphicDescriptorSets = Vector_Alloc(sizeof(VkDescriptorSet), 1);
	pxRenderer->pxParticleGraphicDescriptorSets = Vector_Alloc(sizeof(VkDescriptorSet), 1);
	pxRenderer->pxParticleComputeDescriptorSets = Vector_Alloc(sizeof(VkDescriptorSet), 1);
	pxRenderer->pxPixelComputeDescriptorSets = Vector_Alloc(sizeof(VkDescriptorSet), 1);

	Renderer_AllocDefaultGraphicDescriptorPool(pxRenderer, pxInstance);
	Renderer_AllocParticleGraphicDescriptorPool(pxRenderer, pxInstance);
	Renderer_AllocParticleComputeDescriptorPool(pxRenderer, pxInstance);
	Renderer_AllocPixelComputeDescriptorPool(pxRenderer, pxInstance);

	Renderer_AllocDefaultGraphicDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_AllocParticleGraphicDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_AllocParticleComputeDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_AllocPixelComputeDescriptorSetLayout(pxRenderer, pxInstance);

	Renderer_AllocDefaultPushConstants(pxRenderer);
	Renderer_AllocParticlePushConstants(pxRenderer);

	Renderer_AllocDefaultGraphicPipeline(pxRenderer, pxInstance, pxSwapChain);
	Renderer_AllocParticleGraphicPipeline(pxRenderer, pxInstance, pxSwapChain);
	Renderer_AllocParticleComputePipeline(pxRenderer, pxInstance);
	Renderer_AllocPixelComputePipeline(pxRenderer, pxInstance);

	Renderer_AllocGraphicCommandBuffer(pxRenderer, pxInstance);
	Renderer_AllocComputeCommandBuffer(pxRenderer, pxInstance);

	Renderer_AllocGraphicSynchronizationObjects(pxRenderer, pxInstance);
	Renderer_AllocComputeSynchronizationObjects(pxRenderer, pxInstance);

	return pxRenderer;
}

void Renderer_Free(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	Renderer_FreeComputeSynchronizationObjects(pxRenderer, pxInstance);
	Renderer_FreeGraphicSynchronizationObjects(pxRenderer, pxInstance);

	Renderer_FreePixelComputePipeline(pxRenderer, pxInstance);
	Renderer_FreeParticleComputePipeline(pxRenderer, pxInstance);
	Renderer_FreeParticleGraphicPipeline(pxRenderer, pxInstance);
	Renderer_FreeDefaultGraphicPipeline(pxRenderer, pxInstance);

	Renderer_FreePixelComputeDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_FreeParticleComputeDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_FreeParticleGraphicDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_FreeDefaultGraphicDescriptorSetLayout(pxRenderer, pxInstance);

	Renderer_FreePixelComputeDescriptorPool(pxRenderer, pxInstance);
	Renderer_FreeParticleComputeDescriptorPool(pxRenderer, pxInstance);
	Renderer_FreeParticleGraphicDescriptorPool(pxRenderer, pxInstance);
	Renderer_FreeDefaultGraphicDescriptorPool(pxRenderer, pxInstance);

	Vector_Free(pxRenderer->pxPixelComputeDescriptorSets);
	Vector_Free(pxRenderer->pxParticleComputeDescriptorSets);
	Vector_Free(pxRenderer->pxParticleGraphicDescriptorSets);
	Vector_Free(pxRenderer->pxDefaultGraphicDescriptorSets);

	Buffer_Free(pxRenderer->pxViewProjectionBuffer, pxInstance);
	Buffer_Free(pxRenderer->pxTimeInfoBuffer, pxInstance);

	free(pxRenderer);
}

xTimeInfo_t* Renderer_GetTimeInfo(struct xRenderer_t* pxRenderer) {
	return &pxRenderer->xTimeInfo;
}

xViewProjection_t* Renderer_GetViewProjection(struct xRenderer_t* pxRenderer) {
	return &pxRenderer->xViewProjection;
}

void Renderer_Draw(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xList_t* pxEntities) {
	Renderer_SubmitCompute(pxRenderer, pxInstance, pxEntities);
	Renderer_SubmitGraphic(pxRenderer, pxInstance, pxSwapChain, pxEntities);
}

void Renderer_RebuildEnitityDescriptorSets(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xList_t* pxEntities) {
	uint32_t nDefaultGraphicDescriptorCount = 0;
	uint32_t nParticleGraphicDescriptorCount = 0;
	uint32_t nParticleComputeDescriptorCount = 0;
	uint32_t nPixelComputeDescriptorCount = 0;

	{
		void* pIter = List_Begin(pxEntities);
		while (pIter) {
			struct xEntity_t** ppxEntity = List_Value(pIter);

			if (Entity_HasComponents(*ppxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT)) {
				nDefaultGraphicDescriptorCount++;
			}

			if (Entity_HasComponents(*ppxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT | COMPONENT_PARTICLESYSTEM_BIT)) {
				nParticleGraphicDescriptorCount++;
			}

			if (Entity_HasComponents(*ppxEntity, COMPONENT_PARTICLESYSTEM_BIT)) {
				nParticleComputeDescriptorCount++;
			}

			if (Entity_HasComponents(*ppxEntity, COMPONENT_PIXELSYSTEM_BIT)) {
				nPixelComputeDescriptorCount++;
			}

			pIter = List_Next(pIter);
		}
	}

	if (nDefaultGraphicDescriptorCount == 0) nDefaultGraphicDescriptorCount = 1;
	if (nParticleGraphicDescriptorCount == 0) nParticleGraphicDescriptorCount = 1;
	if (nParticleComputeDescriptorCount == 0) nParticleComputeDescriptorCount = 1;
	if (nPixelComputeDescriptorCount == 0) nPixelComputeDescriptorCount = 1;

	Vector_Resize(pxRenderer->pxDefaultGraphicDescriptorSets, nDefaultGraphicDescriptorCount);
	Vector_Resize(pxRenderer->pxParticleGraphicDescriptorSets, nParticleGraphicDescriptorCount);
	Vector_Resize(pxRenderer->pxParticleComputeDescriptorSets, nParticleComputeDescriptorCount);
	Vector_Resize(pxRenderer->pxPixelComputeDescriptorSets, nPixelComputeDescriptorCount);

#ifdef DEBUG
	printf("Creating %u DefaultGraphicDescriptorSets\n", Vector_Count(pxRenderer->pxDefaultGraphicDescriptorSets));
	printf("Creating %u ParticleGraphicDescriptorSets\n", Vector_Count(pxRenderer->pxParticleGraphicDescriptorSets));
	printf("Creating %u ParticleComputeDescriptorSets\n", Vector_Count(pxRenderer->pxParticleComputeDescriptorSets));
	printf("Creating %u PixelComputeDescriptorSets\n", Vector_Count(pxRenderer->pxPixelComputeDescriptorSets));
#endif

	Renderer_CreateDefaultGraphicDescriptorSets(pxRenderer, pxInstance, nDefaultGraphicDescriptorCount);
	Renderer_CreateParticleGraphicDescriptorSets(pxRenderer, pxInstance, nParticleGraphicDescriptorCount);
	Renderer_CreateParticleComputeDescriptorSets(pxRenderer, pxInstance, nParticleComputeDescriptorCount);
	Renderer_CreatePixelComputeDescriptorSets(pxRenderer, pxInstance, nPixelComputeDescriptorCount);

	uint32_t nDefaultGraphicDescriptorIndex = 0;
	uint32_t nParticleGraphicDescriptorIndex = 0;
	uint32_t nParticleComputeDescriptorIndex = 0;
	uint32_t nPixelComputeDescriptorIndex = 0;

	{
		void* pIter = List_Begin(pxEntities);
		while (pIter) {
			struct xEntity_t** ppxEntity = List_Value(pIter);
	
			if (Entity_HasComponents(*ppxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT)) {
				xRenderable_t* pxRenderable = Entity_GetRenderable(*ppxEntity);

				Renderer_UpdateDefaultGraphicDescriptorSet(pxRenderer, pxInstance, nDefaultGraphicDescriptorIndex, pxRenderable);
				nDefaultGraphicDescriptorIndex++;
			}

			if (Entity_HasComponents(*ppxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT | COMPONENT_PARTICLESYSTEM_BIT)) {
				xRenderable_t* pxRenderable = Entity_GetRenderable(*ppxEntity);
				xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(*ppxEntity);

				Renderer_UpdateParticleGraphicDescriptorSet(pxRenderer, pxInstance, nParticleGraphicDescriptorIndex, pxRenderable, pxParticleSystem);
				nParticleGraphicDescriptorIndex++;
			}
	
			if (Entity_HasComponents(*ppxEntity, COMPONENT_PARTICLESYSTEM_BIT)) {
				xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(*ppxEntity);

				Renderer_UpdateParticleComputeDescriptorSet(pxRenderer, pxInstance, nParticleComputeDescriptorIndex, pxParticleSystem);
				nParticleComputeDescriptorIndex++;
			}

			if (Entity_HasComponents(*ppxEntity, COMPONENT_PIXELSYSTEM_BIT)) {
				xPixelSystem_t* pxPixelSystem = Entity_GetPixelSystem(*ppxEntity);

				Renderer_UpdatePixelComputeDescriptorSet(pxRenderer, pxInstance, nPixelComputeDescriptorIndex, pxPixelSystem);
				nPixelComputeDescriptorIndex++;
			}

			pIter = List_Next(pIter);
		}
	}
}
