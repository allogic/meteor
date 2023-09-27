#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <common/macros.h>

#include <container/list.h>
#include <container/queue.h>
#include <container/vector.h>

#include <math/vector.h>
#include <math/matrix.h>

#include <platform/nativewindow.h>

#include <vulkan/instance.h>
#include <vulkan/swapchain.h>
#include <vulkan/shader.h>
#include <vulkan/graphicpipeline.h>
#include <vulkan/computepipeline.h>
#include <vulkan/vertex.h>
#include <vulkan/uniform.h>
#include <vulkan/pushconstants.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>
#include <vulkan/image.h>

#include <game/renderer.h>
#include <game/component.h>
#include <game/entity.h>
#include <game/pushconstants.h>
#include <game/storage.h>

#define DEFAULT_VERTEX_BINDING_ID 0
#define INSTANCE_VERTEX_BINDING_ID 1

#define INITIAL_DEFAULT_GRAPHIC_POOL_COUNT 1024
#define INITIAL_PARTICLE_GRAPHIC_POOL_COUNT 1024
#define INITIAL_PARTICLE_COMPUTE_POOL_COUNT 1024
#define INITIAL_PIXEL_COMPUTE_POOL_COUNT 1024

#define DEBUG_VERTEX_BUFFER_COUNT (65535)
#define DEBUG_INDEX_BUFFER_COUNT (DEBUG_VERTEX_BUFFER_COUNT * 2)

struct xRenderer_t {
	xTimeInfo_t xTimeInfo;
	xViewProjection_t xViewProjection;

	struct xBuffer_t* pxTimeInfoBuffer;
	struct xBuffer_t* pxViewProjectionBuffer;

	VkDescriptorPool xDefaultGraphicDescriptorPool;
	VkDescriptorPool xParticleGraphicDescriptorPool;
	VkDescriptorPool xParticleComputeDescriptorPool;
	VkDescriptorPool xPixelComputeDescriptorPool;
	VkDescriptorPool xDebugGraphicDescriptorPool;

	VkDescriptorSetLayout xDefaultGraphicDescriptorSetLayout;
	VkDescriptorSetLayout xParticleGraphicDescriptorSetLayout;
	VkDescriptorSetLayout xParticleComputeDescriptorSetLayout;
	VkDescriptorSetLayout xPixelComputeDescriptorSetLayout;
	VkDescriptorSetLayout xDebugGraphicDescriptorSetLayout;

	struct xVector_t* pxDefaultGraphicDescriptorSets;
	struct xVector_t* pxParticleGraphicDescriptorSets;
	struct xVector_t* pxParticleComputeDescriptorSets;
	struct xVector_t* pxPixelComputeDescriptorSets;
	VkDescriptorSet xDebugGraphicDescriptorSet;

	struct xVector_t* pxDefaultGraphicEntities;
	struct xVector_t* pxParticleGraphicEntities;
	struct xVector_t* pxParticleComputeEntities;
	struct xVector_t* pxPixelComputeEntities;

	VkPushConstantRange axDefaultGraphicPushConstantRanges[1];
	VkPushConstantRange axParticleGraphicPushConstantRanges[1];
	VkPushConstantRange axParticleComputePushConstantRanges[1];
	VkPushConstantRange axPixelComputePushConstantRanges[1];

	struct xGraphicPipeline_t* pxDefaultGraphicPipeline;
	struct xGraphicPipeline_t* pxParticleGraphicPipeline;
	struct xComputePipeline_t* pxParticleComputePipeline;
	struct xComputePipeline_t* pxPixelComputePipeline;
	struct xGraphicPipeline_t* pxDebugGraphicPipeline;

	VkCommandBuffer xGraphicCommandBuffer;
	VkCommandBuffer xComputeCommandBuffer;

	VkSemaphore xGraphicCompleteSemaphore;
	VkSemaphore xComputeCompleteSemaphore;
	VkSemaphore xImageAvailableSemaphore;

	VkFence xRenderFence;

	struct xBuffer_t* pxDebugVertexBuffer;
	struct xBuffer_t* pxDebugIndexBuffer;

	xDebugVertex_t* pxDebugVertices;
	uint32_t* pnDebugIndices;

	uint32_t nDebugVertexCount;
	uint32_t nDebugIndexCount;
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
	xDescriptorPoolCreateInfo.maxSets = INITIAL_PARTICLE_GRAPHIC_POOL_COUNT;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xParticleGraphicDescriptorPool));
}
static void Renderer_AllocParticleComputeDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorPoolSize axDescriptorPoolSizes[3];

	axDescriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorPoolSizes[0].descriptorCount = 1;

	axDescriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorPoolSizes[1].descriptorCount = 1;

	axDescriptorPoolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axDescriptorPoolSizes[2].descriptorCount = 2;

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
	axDescriptorPoolSizes[1].descriptorCount = 12;

	VkDescriptorPoolCreateInfo xDescriptorPoolCreateInfo;
	memset(&xDescriptorPoolCreateInfo, 0, sizeof(xDescriptorPoolCreateInfo));
	xDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	xDescriptorPoolCreateInfo.poolSizeCount = ARRAY_LENGTH(axDescriptorPoolSizes);
	xDescriptorPoolCreateInfo.pPoolSizes = axDescriptorPoolSizes;
	xDescriptorPoolCreateInfo.maxSets = INITIAL_PIXEL_COMPUTE_POOL_COUNT;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xPixelComputeDescriptorPool));
}
static void Renderer_AllocDebugGraphicDescriptorPool(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorPoolSize axDescriptorPoolSizes[1];

	axDescriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorPoolSizes[0].descriptorCount = 1;

	VkDescriptorPoolCreateInfo xDescriptorPoolCreateInfo;
	memset(&xDescriptorPoolCreateInfo, 0, sizeof(xDescriptorPoolCreateInfo));
	xDescriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	xDescriptorPoolCreateInfo.poolSizeCount = ARRAY_LENGTH(axDescriptorPoolSizes);
	xDescriptorPoolCreateInfo.pPoolSizes = axDescriptorPoolSizes;
	xDescriptorPoolCreateInfo.maxSets = 1;

	VK_CHECK(vkCreateDescriptorPool(Instance_GetDevice(pxInstance), &xDescriptorPoolCreateInfo, 0, &pxRenderer->xDebugGraphicDescriptorPool));
}

static void Renderer_FreeDescriptorPools(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xDefaultGraphicDescriptorPool, 0);
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xParticleGraphicDescriptorPool, 0);
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xParticleComputeDescriptorPool, 0);
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xPixelComputeDescriptorPool, 0);
	vkDestroyDescriptorPool(Instance_GetDevice(pxInstance), pxRenderer->xDebugGraphicDescriptorPool, 0);
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

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xParticleGraphicDescriptorSetLayout));
}
static void Renderer_AllocParticleComputeDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[4];

	axDescriptorSetLayoutBindings[0].binding = 0;
	axDescriptorSetLayoutBindings[0].descriptorCount = 1;
	axDescriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorSetLayoutBindings[0].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[1].binding = 1;
	axDescriptorSetLayoutBindings[1].descriptorCount = 1;
	axDescriptorSetLayoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorSetLayoutBindings[1].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[2].binding = 2;
	axDescriptorSetLayoutBindings[2].descriptorCount = 1;
	axDescriptorSetLayoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axDescriptorSetLayoutBindings[2].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[3].binding = 3;
	axDescriptorSetLayoutBindings[3].descriptorCount = 1;
	axDescriptorSetLayoutBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axDescriptorSetLayoutBindings[3].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[3].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = ARRAY_LENGTH(axDescriptorSetLayoutBindings);
	xDescriptorSetLayoutCreateInfo.pBindings = axDescriptorSetLayoutBindings;

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xParticleComputeDescriptorSetLayout));
}
static void Renderer_AllocPixelComputeDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[13];

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

	axDescriptorSetLayoutBindings[6].binding = 6;
	axDescriptorSetLayoutBindings[6].descriptorCount = 1;
	axDescriptorSetLayoutBindings[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[6].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[6].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[7].binding = 7;
	axDescriptorSetLayoutBindings[7].descriptorCount = 1;
	axDescriptorSetLayoutBindings[7].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[7].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[7].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[8].binding = 8;
	axDescriptorSetLayoutBindings[8].descriptorCount = 1;
	axDescriptorSetLayoutBindings[8].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[8].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[8].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[9].binding = 9;
	axDescriptorSetLayoutBindings[9].descriptorCount = 1;
	axDescriptorSetLayoutBindings[9].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[9].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[9].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[10].binding = 10;
	axDescriptorSetLayoutBindings[10].descriptorCount = 1;
	axDescriptorSetLayoutBindings[10].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[10].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[10].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[11].binding = 11;
	axDescriptorSetLayoutBindings[11].descriptorCount = 1;
	axDescriptorSetLayoutBindings[11].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[11].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[11].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	axDescriptorSetLayoutBindings[12].binding = 12;
	axDescriptorSetLayoutBindings[12].descriptorCount = 1;
	axDescriptorSetLayoutBindings[12].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axDescriptorSetLayoutBindings[12].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[12].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = ARRAY_LENGTH(axDescriptorSetLayoutBindings);
	xDescriptorSetLayoutCreateInfo.pBindings = axDescriptorSetLayoutBindings;

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xPixelComputeDescriptorSetLayout));
}
static void Renderer_AllocDebugGraphicDescriptorSetLayout(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[1];

	axDescriptorSetLayoutBindings[0].binding = 0;
	axDescriptorSetLayoutBindings[0].descriptorCount = 1;
	axDescriptorSetLayoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axDescriptorSetLayoutBindings[0].pImmutableSamplers = 0;
	axDescriptorSetLayoutBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo xDescriptorSetLayoutCreateInfo;
	memset(&xDescriptorSetLayoutCreateInfo, 0, sizeof(xDescriptorSetLayoutCreateInfo));
	xDescriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	xDescriptorSetLayoutCreateInfo.bindingCount = ARRAY_LENGTH(axDescriptorSetLayoutBindings);
	xDescriptorSetLayoutCreateInfo.pBindings = axDescriptorSetLayoutBindings;

	VK_CHECK(vkCreateDescriptorSetLayout(Instance_GetDevice(pxInstance), &xDescriptorSetLayoutCreateInfo, 0, &pxRenderer->xDebugGraphicDescriptorSetLayout));
}

static void Renderer_FreeDescriptorSetLayouts(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xDefaultGraphicDescriptorSetLayout, 0);
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xParticleGraphicDescriptorSetLayout, 0);
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xParticleComputeDescriptorSetLayout, 0);
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xPixelComputeDescriptorSetLayout, 0);
	vkDestroyDescriptorSetLayout(Instance_GetDevice(pxInstance), pxRenderer->xDebugGraphicDescriptorSetLayout, 0);
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
static void Renderer_CreateDebugGraphicDescriptorSet(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorSetAllocateInfo xDescriptorSetAllocateInfo;
	memset(&xDescriptorSetAllocateInfo, 0, sizeof(xDescriptorSetAllocateInfo));
	xDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	xDescriptorSetAllocateInfo.descriptorPool = pxRenderer->xDebugGraphicDescriptorPool;
	xDescriptorSetAllocateInfo.descriptorSetCount = 1;
	xDescriptorSetAllocateInfo.pSetLayouts = &pxRenderer->xDebugGraphicDescriptorSetLayout;

	VK_CHECK(vkAllocateDescriptorSets(Instance_GetDevice(pxInstance), &xDescriptorSetAllocateInfo, &pxRenderer->xDebugGraphicDescriptorSet));
}

static void Renderer_UpdateDefaultGraphicDescriptorSet(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetIndex, xRenderable_t* pxRenderable) {
	VkDescriptorBufferInfo xViewProjectionDescriptorBufferInfo;
	memset(&xViewProjectionDescriptorBufferInfo, 0, sizeof(xViewProjectionDescriptorBufferInfo));
	xViewProjectionDescriptorBufferInfo.offset = 0;
	xViewProjectionDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxViewProjectionBuffer);
	xViewProjectionDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxViewProjectionBuffer);

	VkDescriptorImageInfo xCombinedImageSamplerDescriptorImageInfo;
	memset(&xCombinedImageSamplerDescriptorImageInfo, 0, sizeof(xCombinedImageSamplerDescriptorImageInfo));
	xCombinedImageSamplerDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xCombinedImageSamplerDescriptorImageInfo.imageView = Image_GetImageView(pxRenderable->pxColorImage);
	xCombinedImageSamplerDescriptorImageInfo.sampler = Image_GetSampler(pxRenderable->pxColorImage);

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
static void Renderer_UpdateParticleGraphicDescriptorSet(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetIndex, xRenderable_t* pxRenderable) {
	VkDescriptorBufferInfo xViewProjectionDescriptorBufferInfo;
	memset(&xViewProjectionDescriptorBufferInfo, 0, sizeof(xViewProjectionDescriptorBufferInfo));
	xViewProjectionDescriptorBufferInfo.offset = 0;
	xViewProjectionDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxViewProjectionBuffer);
	xViewProjectionDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxViewProjectionBuffer);

	VkDescriptorImageInfo xCombinedImageSamplerDescriptorImageInfo;
	memset(&xCombinedImageSamplerDescriptorImageInfo, 0, sizeof(xCombinedImageSamplerDescriptorImageInfo));
	xCombinedImageSamplerDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xCombinedImageSamplerDescriptorImageInfo.imageView = Image_GetImageView(pxRenderable->pxColorImage);
	xCombinedImageSamplerDescriptorImageInfo.sampler = Image_GetSampler(pxRenderable->pxColorImage);

	VkWriteDescriptorSet axWriteDescriptorSets[2];

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
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = &xCombinedImageSamplerDescriptorImageInfo;
	axWriteDescriptorSets[1].pBufferInfo = 0;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}
static void Renderer_UpdateParticleComputeDescriptorSet(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetIndex, xParticleSystem_t* pxParticleSystem) {
	VkDescriptorBufferInfo xTimeInfoDescriptorBufferInfo;
	memset(&xTimeInfoDescriptorBufferInfo, 0, sizeof(xTimeInfoDescriptorBufferInfo));
	xTimeInfoDescriptorBufferInfo.offset = 0;
	xTimeInfoDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxTimeInfoBuffer);
	xTimeInfoDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxTimeInfoBuffer);

	VkDescriptorBufferInfo xParticleBehaviourDescriptorBufferInfo;
	memset(&xParticleBehaviourDescriptorBufferInfo, 0, sizeof(xParticleBehaviourDescriptorBufferInfo));
	xParticleBehaviourDescriptorBufferInfo.offset = 0;
	xParticleBehaviourDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxParticleSystem->pxParticleBehaviour);
	xParticleBehaviourDescriptorBufferInfo.range = Buffer_GetSize(pxParticleSystem->pxParticleBehaviour);

	VkDescriptorBufferInfo xParticleDescriptorBufferInfo;
	memset(&xParticleDescriptorBufferInfo, 0, sizeof(xParticleDescriptorBufferInfo));
	xParticleDescriptorBufferInfo.offset = 0;
	xParticleDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxParticleSystem->pxParticleBuffer);
	xParticleDescriptorBufferInfo.range = Buffer_GetSize(pxParticleSystem->pxParticleBuffer);

	VkWriteDescriptorSet axWriteDescriptorSets[4];

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
	axWriteDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[1].descriptorCount = 1;
	axWriteDescriptorSets[1].pImageInfo = 0;
	axWriteDescriptorSets[1].pBufferInfo = &xParticleBehaviourDescriptorBufferInfo;
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

	axWriteDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[3].pNext = 0;
	axWriteDescriptorSets[3].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxParticleComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[3].dstBinding = 3;
	axWriteDescriptorSets[3].dstArrayElement = 0;
	axWriteDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	axWriteDescriptorSets[3].descriptorCount = 1;
	axWriteDescriptorSets[3].pImageInfo = 0;
	axWriteDescriptorSets[3].pBufferInfo = &xParticleDescriptorBufferInfo;
	axWriteDescriptorSets[3].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}
static void Renderer_UpdatePixelComputeDescriptorSet(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, uint32_t nDescriptorSetIndex, xPixelSystem_t* pxPixelSystem) {
	VkDescriptorBufferInfo xTimeInfoDescriptorBufferInfo;
	memset(&xTimeInfoDescriptorBufferInfo, 0, sizeof(xTimeInfoDescriptorBufferInfo));
	xTimeInfoDescriptorBufferInfo.offset = 0;
	xTimeInfoDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxTimeInfoBuffer);
	xTimeInfoDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxTimeInfoBuffer);

	VkDescriptorImageInfo xNorthColorDescriptorImageInfo;
	memset(&xNorthColorDescriptorImageInfo, 0, sizeof(xNorthColorDescriptorImageInfo));
	xNorthColorDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xNorthColorDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxColorImageN);
	xNorthColorDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxColorImageN);

	VkDescriptorImageInfo xSouthColorDescriptorImageInfo;
	memset(&xSouthColorDescriptorImageInfo, 0, sizeof(xSouthColorDescriptorImageInfo));
	xSouthColorDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xSouthColorDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxColorImageS);
	xSouthColorDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxColorImageS);

	VkDescriptorImageInfo xWestColorDescriptorImageInfo;
	memset(&xWestColorDescriptorImageInfo, 0, sizeof(xWestColorDescriptorImageInfo));
	xWestColorDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xWestColorDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxColorImageW);
	xWestColorDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxColorImageW);

	VkDescriptorImageInfo xEastColorDescriptorImageInfo;
	memset(&xEastColorDescriptorImageInfo, 0, sizeof(xEastColorDescriptorImageInfo));
	xEastColorDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xEastColorDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxColorImageE);
	xEastColorDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxColorImageE);

	VkDescriptorImageInfo xColorDescriptorImageInfo;
	memset(&xColorDescriptorImageInfo, 0, sizeof(xColorDescriptorImageInfo));
	xColorDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xColorDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxColorImage);
	xColorDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxColorImage);

	VkDescriptorImageInfo xNorthStateDescriptorImageInfo;
	memset(&xNorthStateDescriptorImageInfo, 0, sizeof(xNorthStateDescriptorImageInfo));
	xNorthStateDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xNorthStateDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxStateImageN);
	xNorthStateDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxStateImageN);

	VkDescriptorImageInfo xSouthStateDescriptorImageInfo;
	memset(&xSouthStateDescriptorImageInfo, 0, sizeof(xSouthStateDescriptorImageInfo));
	xSouthStateDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xSouthStateDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxStateImageS);
	xSouthStateDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxStateImageS);

	VkDescriptorImageInfo xWestStateDescriptorImageInfo;
	memset(&xWestStateDescriptorImageInfo, 0, sizeof(xWestStateDescriptorImageInfo));
	xWestStateDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xWestStateDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxStateImageW);
	xWestStateDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxStateImageW);

	VkDescriptorImageInfo xEastStateDescriptorImageInfo;
	memset(&xEastStateDescriptorImageInfo, 0, sizeof(xEastStateDescriptorImageInfo));
	xEastStateDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xEastStateDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxStateImageE);
	xEastStateDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxStateImageE);

	VkDescriptorImageInfo xStateDescriptorImageInfo;
	memset(&xStateDescriptorImageInfo, 0, sizeof(xStateDescriptorImageInfo));
	xStateDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	xStateDescriptorImageInfo.imageView = Image_GetImageView(pxPixelSystem->pxStateImage);
	xStateDescriptorImageInfo.sampler = Image_GetSampler(pxPixelSystem->pxStateImage);

	VkWriteDescriptorSet axWriteDescriptorSets[13];

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
	axWriteDescriptorSets[1].pImageInfo = &xNorthColorDescriptorImageInfo;
	axWriteDescriptorSets[1].pBufferInfo = 0;
	axWriteDescriptorSets[1].pTexelBufferView = 0;

	axWriteDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[2].pNext = 0;
	axWriteDescriptorSets[2].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[2].dstBinding = 2;
	axWriteDescriptorSets[2].dstArrayElement = 0;
	axWriteDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[2].descriptorCount = 1;
	axWriteDescriptorSets[2].pImageInfo = &xSouthColorDescriptorImageInfo;
	axWriteDescriptorSets[2].pBufferInfo = 0;
	axWriteDescriptorSets[2].pTexelBufferView = 0;

	axWriteDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[3].pNext = 0;
	axWriteDescriptorSets[3].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[3].dstBinding = 3;
	axWriteDescriptorSets[3].dstArrayElement = 0;
	axWriteDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[3].descriptorCount = 1;
	axWriteDescriptorSets[3].pImageInfo = &xWestColorDescriptorImageInfo;
	axWriteDescriptorSets[3].pBufferInfo = 0;
	axWriteDescriptorSets[3].pTexelBufferView = 0;

	axWriteDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[4].pNext = 0;
	axWriteDescriptorSets[4].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[4].dstBinding = 4;
	axWriteDescriptorSets[4].dstArrayElement = 0;
	axWriteDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[4].descriptorCount = 1;
	axWriteDescriptorSets[4].pImageInfo = &xEastColorDescriptorImageInfo;
	axWriteDescriptorSets[4].pBufferInfo = 0;
	axWriteDescriptorSets[4].pTexelBufferView = 0;

	axWriteDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[5].pNext = 0;
	axWriteDescriptorSets[5].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[5].dstBinding = 5;
	axWriteDescriptorSets[5].dstArrayElement = 0;
	axWriteDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[5].descriptorCount = 1;
	axWriteDescriptorSets[5].pImageInfo = &xColorDescriptorImageInfo;
	axWriteDescriptorSets[5].pBufferInfo = 0;
	axWriteDescriptorSets[5].pTexelBufferView = 0;

	axWriteDescriptorSets[6].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[6].pNext = 0;
	axWriteDescriptorSets[6].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[6].dstBinding = 6;
	axWriteDescriptorSets[6].dstArrayElement = 0;
	axWriteDescriptorSets[6].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[6].descriptorCount = 1;
	axWriteDescriptorSets[6].pImageInfo = &xNorthStateDescriptorImageInfo;
	axWriteDescriptorSets[6].pBufferInfo = 0;
	axWriteDescriptorSets[6].pTexelBufferView = 0;

	axWriteDescriptorSets[7].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[7].pNext = 0;
	axWriteDescriptorSets[7].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[7].dstBinding = 7;
	axWriteDescriptorSets[7].dstArrayElement = 0;
	axWriteDescriptorSets[7].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[7].descriptorCount = 1;
	axWriteDescriptorSets[7].pImageInfo = &xSouthStateDescriptorImageInfo;
	axWriteDescriptorSets[7].pBufferInfo = 0;
	axWriteDescriptorSets[7].pTexelBufferView = 0;

	axWriteDescriptorSets[8].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[8].pNext = 0;
	axWriteDescriptorSets[8].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[8].dstBinding = 8;
	axWriteDescriptorSets[8].dstArrayElement = 0;
	axWriteDescriptorSets[8].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[8].descriptorCount = 1;
	axWriteDescriptorSets[8].pImageInfo = &xWestStateDescriptorImageInfo;
	axWriteDescriptorSets[8].pBufferInfo = 0;
	axWriteDescriptorSets[8].pTexelBufferView = 0;

	axWriteDescriptorSets[9].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[9].pNext = 0;
	axWriteDescriptorSets[9].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[9].dstBinding = 9;
	axWriteDescriptorSets[9].dstArrayElement = 0;
	axWriteDescriptorSets[9].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[9].descriptorCount = 1;
	axWriteDescriptorSets[9].pImageInfo = &xEastStateDescriptorImageInfo;
	axWriteDescriptorSets[9].pBufferInfo = 0;
	axWriteDescriptorSets[9].pTexelBufferView = 0;

	axWriteDescriptorSets[10].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[10].pNext = 0;
	axWriteDescriptorSets[10].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[10].dstBinding = 10;
	axWriteDescriptorSets[10].dstArrayElement = 0;
	axWriteDescriptorSets[10].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[10].descriptorCount = 1;
	axWriteDescriptorSets[10].pImageInfo = &xStateDescriptorImageInfo;
	axWriteDescriptorSets[10].pBufferInfo = 0;
	axWriteDescriptorSets[10].pTexelBufferView = 0;

	axWriteDescriptorSets[11].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[11].pNext = 0;
	axWriteDescriptorSets[11].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[11].dstBinding = 11;
	axWriteDescriptorSets[11].dstArrayElement = 0;
	axWriteDescriptorSets[11].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[11].descriptorCount = 1;
	axWriteDescriptorSets[11].pImageInfo = &xColorDescriptorImageInfo;
	axWriteDescriptorSets[11].pBufferInfo = 0;
	axWriteDescriptorSets[11].pTexelBufferView = 0;

	axWriteDescriptorSets[12].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[12].pNext = 0;
	axWriteDescriptorSets[12].dstSet = *(VkDescriptorSet*)Vector_At(pxRenderer->pxPixelComputeDescriptorSets, nDescriptorSetIndex);
	axWriteDescriptorSets[12].dstBinding = 12;
	axWriteDescriptorSets[12].dstArrayElement = 0;
	axWriteDescriptorSets[12].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	axWriteDescriptorSets[12].descriptorCount = 1;
	axWriteDescriptorSets[12].pImageInfo = &xStateDescriptorImageInfo;
	axWriteDescriptorSets[12].pBufferInfo = 0;
	axWriteDescriptorSets[12].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}
static void Renderer_UpdateDebugGraphicDescriptorSet(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkDescriptorBufferInfo xViewProjectionDescriptorBufferInfo;
	memset(&xViewProjectionDescriptorBufferInfo, 0, sizeof(xViewProjectionDescriptorBufferInfo));
	xViewProjectionDescriptorBufferInfo.offset = 0;
	xViewProjectionDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxViewProjectionBuffer);
	xViewProjectionDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxViewProjectionBuffer);

	VkWriteDescriptorSet axWriteDescriptorSets[1];

	axWriteDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	axWriteDescriptorSets[0].pNext = 0;
	axWriteDescriptorSets[0].dstSet = pxRenderer->xDebugGraphicDescriptorSet;
	axWriteDescriptorSets[0].dstBinding = 0;
	axWriteDescriptorSets[0].dstArrayElement = 0;
	axWriteDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	axWriteDescriptorSets[0].descriptorCount = 1;
	axWriteDescriptorSets[0].pImageInfo = 0;
	axWriteDescriptorSets[0].pBufferInfo = &xViewProjectionDescriptorBufferInfo;
	axWriteDescriptorSets[0].pTexelBufferView = 0;

	vkUpdateDescriptorSets(Instance_GetDevice(pxInstance), ARRAY_LENGTH(axWriteDescriptorSets), axWriteDescriptorSets, 0, 0);
}

static void Renderer_AllocDefaultGraphicPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	VkShaderModule xVertModule = Shader_Alloc(pxInstance, "shaders/default.vert.spv");
	VkShaderModule xFragModule = Shader_Alloc(pxInstance, "shaders/default.frag.spv");

	VkVertexInputBindingDescription axVertexInputBindingDescriptions[1];

	axVertexInputBindingDescriptions[0].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputBindingDescriptions[0].stride = sizeof(xDefaultVertex_t);
	axVertexInputBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[3];

	axVertexInputAttributeDescriptions[0].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputAttributeDescriptions[0].location = 0;
	axVertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	axVertexInputAttributeDescriptions[0].offset = 0;

	axVertexInputAttributeDescriptions[1].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputAttributeDescriptions[1].location = 1;
	axVertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	axVertexInputAttributeDescriptions[1].offset = 3 * sizeof(float);

	axVertexInputAttributeDescriptions[2].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputAttributeDescriptions[2].location = 2;
	axVertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	axVertexInputAttributeDescriptions[2].offset = 5 * sizeof(float);

	pxRenderer->pxDefaultGraphicPipeline = GraphicPipeline_Alloc(
		pxInstance,
		pxSwapChain,
		xVertModule,
		xFragModule,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		axVertexInputBindingDescriptions,
		ARRAY_LENGTH(axVertexInputBindingDescriptions),
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

	VkVertexInputBindingDescription axVertexInputBindingDescriptions[2];

	axVertexInputBindingDescriptions[0].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputBindingDescriptions[0].stride = sizeof(xDefaultVertex_t);
	axVertexInputBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	axVertexInputBindingDescriptions[1].binding = INSTANCE_VERTEX_BINDING_ID;
	axVertexInputBindingDescriptions[1].stride = sizeof(xParticle_t);
	axVertexInputBindingDescriptions[1].inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[5];

	axVertexInputAttributeDescriptions[0].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputAttributeDescriptions[0].location = 0;
	axVertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	axVertexInputAttributeDescriptions[0].offset = 0;

	axVertexInputAttributeDescriptions[1].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputAttributeDescriptions[1].location = 1;
	axVertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	axVertexInputAttributeDescriptions[1].offset = 3 * sizeof(float);

	axVertexInputAttributeDescriptions[2].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputAttributeDescriptions[2].location = 2;
	axVertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	axVertexInputAttributeDescriptions[2].offset = 5 * sizeof(float);

	axVertexInputAttributeDescriptions[3].binding = INSTANCE_VERTEX_BINDING_ID;
	axVertexInputAttributeDescriptions[3].location = 3;
	axVertexInputAttributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	axVertexInputAttributeDescriptions[3].offset = 0;

	axVertexInputAttributeDescriptions[4].binding = INSTANCE_VERTEX_BINDING_ID;
	axVertexInputAttributeDescriptions[4].location = 4;
	axVertexInputAttributeDescriptions[4].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	axVertexInputAttributeDescriptions[4].offset = 4 * sizeof(float);

	pxRenderer->pxParticleGraphicPipeline = GraphicPipeline_Alloc(
		pxInstance,
		pxSwapChain,
		xVertModule,
		xFragModule,
		VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		axVertexInputBindingDescriptions,
		ARRAY_LENGTH(axVertexInputBindingDescriptions),
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
		pxRenderer->xParticleComputeDescriptorSetLayout,
		pxRenderer->axParticleComputePushConstantRanges,
		ARRAY_LENGTH(pxRenderer->axParticleComputePushConstantRanges));

	Shader_Free(pxInstance, xCompModule);
}
static void Renderer_AllocPixelComputePipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkShaderModule xCompModule = Shader_Alloc(pxInstance, "shaders/pixel.comp.spv");

	pxRenderer->pxPixelComputePipeline = ComputePipeline_Alloc(
		pxInstance,
		xCompModule,
		pxRenderer->xPixelComputeDescriptorSetLayout,
		pxRenderer->axPixelComputePushConstantRanges,
		ARRAY_LENGTH(pxRenderer->axPixelComputePushConstantRanges));

	Shader_Free(pxInstance, xCompModule);
}
static void Renderer_AllocDebugGraphicPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	VkShaderModule xVertModule = Shader_Alloc(pxInstance, "shaders/debug.vert.spv");
	VkShaderModule xFragModule = Shader_Alloc(pxInstance, "shaders/debug.frag.spv");

	VkVertexInputBindingDescription axVertexInputBindingDescriptions[1];

	axVertexInputBindingDescriptions[0].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputBindingDescriptions[0].stride = sizeof(xDebugVertex_t);
	axVertexInputBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[2];

	axVertexInputAttributeDescriptions[0].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputAttributeDescriptions[0].location = 0;
	axVertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	axVertexInputAttributeDescriptions[0].offset = 0;

	axVertexInputAttributeDescriptions[1].binding = DEFAULT_VERTEX_BINDING_ID;
	axVertexInputAttributeDescriptions[1].location = 1;
	axVertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	axVertexInputAttributeDescriptions[1].offset = 3 * sizeof(float);

	pxRenderer->pxDebugGraphicPipeline = GraphicPipeline_Alloc(
		pxInstance,
		pxSwapChain,
		xVertModule,
		xFragModule,
		VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
		axVertexInputBindingDescriptions,
		ARRAY_LENGTH(axVertexInputBindingDescriptions),
		axVertexInputAttributeDescriptions,
		ARRAY_LENGTH(axVertexInputAttributeDescriptions),
		pxRenderer->xDebugGraphicDescriptorSetLayout,
		0,
		0);

	Shader_Free(pxInstance, xVertModule);
	Shader_Free(pxInstance, xFragModule);
}

static void Renderer_FreePipelines(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	GraphicPipeline_Free(pxRenderer->pxDefaultGraphicPipeline, pxInstance);
	GraphicPipeline_Free(pxRenderer->pxParticleGraphicPipeline, pxInstance);
	ComputePipeline_Free(pxRenderer->pxParticleComputePipeline, pxInstance);
	ComputePipeline_Free(pxRenderer->pxPixelComputePipeline, pxInstance);
	GraphicPipeline_Free(pxRenderer->pxDebugGraphicPipeline, pxInstance);
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

static void Renderer_FreeCommandBuffers(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkFreeCommandBuffers(Instance_GetDevice(pxInstance), Instance_GetCommandPool(pxInstance), 1, &pxRenderer->xGraphicCommandBuffer);
	vkFreeCommandBuffers(Instance_GetDevice(pxInstance), Instance_GetCommandPool(pxInstance), 1, &pxRenderer->xComputeCommandBuffer);
}

static void Renderer_BuildGraphicCommandBuffer(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, uint32_t nImageIndex) {
	Instance_GraphicQueueWaitIdle(pxInstance);

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

		for (uint32_t i = 0; i < Vector_Count(pxRenderer->pxDefaultGraphicEntities); ++i) {
			struct xEntity_t* pxEntity = *(struct xEntity_t**)Vector_At(pxRenderer->pxDefaultGraphicEntities, i);

			xTransform_t* pxTransform = Entity_GetTransform(pxEntity);
			xRenderable_t* pxRenderable = Entity_GetRenderable(pxEntity);

			VkBuffer axDefaultVertexBuffers[] = { Buffer_GetBuffer(pxRenderable->pxVertexBuffer) };

			uint64_t awDefaultOffsets[] = { 0 };

			vkCmdBindDescriptorSets(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipelineLayout(pxRenderer->pxDefaultGraphicPipeline), 0, 1, Vector_At(pxRenderer->pxDefaultGraphicDescriptorSets, i), 0, 0);
			vkCmdBindVertexBuffers(pxRenderer->xGraphicCommandBuffer, DEFAULT_VERTEX_BINDING_ID, 1, axDefaultVertexBuffers, awDefaultOffsets);
			vkCmdBindIndexBuffer(pxRenderer->xGraphicCommandBuffer, Buffer_GetBuffer(pxRenderable->pxIndexBuffer), 0, VK_INDEX_TYPE_UINT32);

			static xPerEntityData_t xPerEntityData;
			memset(&xPerEntityData, 0, sizeof(xPerEntityData));

			// TODO: Maybe also do this on the GPU (compute shader and linear transformation buffer)
			Matrix4_Identity(xPerEntityData.xModel);
			Matrix4_SetPosition(xPerEntityData.xModel, pxTransform->xPosition);
			//Matrix4_RotateX(xPerEntityData.xModel, pxTransform->xRotation[0]);
			//Matrix4_RotateY(xPerEntityData.xModel, pxTransform->xRotation[0]);
			//Matrix4_RotateZ(xPerEntityData.xModel, pxTransform->xRotation[0]);
			Matrix4_SetScale(xPerEntityData.xModel, pxTransform->xScale);

			vkCmdPushConstants(pxRenderer->xGraphicCommandBuffer, GraphicPipeline_GetPipelineLayout(pxRenderer->pxDefaultGraphicPipeline), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(xPerEntityData), &xPerEntityData);

			vkCmdDrawIndexed(pxRenderer->xGraphicCommandBuffer, pxRenderable->nIndexCount, 1, 0, 0, 0);
		}
	}

	{
		vkCmdBindPipeline(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipeline(pxRenderer->pxParticleGraphicPipeline));

		vkCmdSetViewport(pxRenderer->xGraphicCommandBuffer, 0, 1, &xViewport);
		vkCmdSetScissor(pxRenderer->xGraphicCommandBuffer, 0, 1, &xScissor);

		for (uint32_t i = 0; i < Vector_Count(pxRenderer->pxParticleGraphicEntities); ++i) {
			struct xEntity_t* pxEntity = *(struct xEntity_t**)Vector_At(pxRenderer->pxParticleGraphicEntities, i);

			xTransform_t* pxTransform = Entity_GetTransform(pxEntity);
			xRenderable_t* pxRenderable = Entity_GetRenderable(pxEntity);
			xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(pxEntity);

			if (pxParticleSystem->bDebug) {
				xVec3_t xPosition = { pxTransform->xPosition[0], pxTransform->xPosition[1], pxTransform->xPosition[2] };
				xVec3_t xSize = { pxParticleSystem->fWidth, pxParticleSystem->fHeight, pxParticleSystem->fDepth };
				xVec4_t xColor = { 1.0F, 0.0F, 0.0F, 1.0F };
				xVec4_t xRotation = { 0.0F, 0.0F, 0.0F, 0.0F };

				Renderer_DrawDebugBox(pxRenderer, xPosition, xSize, xColor, xRotation);
			}

			VkBuffer axDefaultVertexBuffers[] = { Buffer_GetBuffer(pxRenderable->pxVertexBuffer) };
			VkBuffer axInstanceVertexBuffers[] = { Buffer_GetBuffer(pxParticleSystem->pxParticleBuffer) };

			uint64_t awDefaultOffsets[] = { 0 };
			uint64_t awInstanceOffsets[] = { 0 };

			vkCmdBindDescriptorSets(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipelineLayout(pxRenderer->pxParticleGraphicPipeline), 0, 1, Vector_At(pxRenderer->pxParticleGraphicDescriptorSets, i), 0, 0);
			vkCmdBindVertexBuffers(pxRenderer->xGraphicCommandBuffer, DEFAULT_VERTEX_BINDING_ID, 1, axDefaultVertexBuffers, awDefaultOffsets);
			vkCmdBindVertexBuffers(pxRenderer->xGraphicCommandBuffer, INSTANCE_VERTEX_BINDING_ID, 1, axInstanceVertexBuffers, awInstanceOffsets);
			vkCmdBindIndexBuffer(pxRenderer->xGraphicCommandBuffer, Buffer_GetBuffer(pxRenderable->pxIndexBuffer), 0, VK_INDEX_TYPE_UINT32);

			static xPerEntityData_t xPerEntityData;
			memset(&xPerEntityData, 0, sizeof(xPerEntityData));

			// TODO: Maybe also do this on the GPU (compute shader and linear transformation buffer)
			Matrix4_Identity(xPerEntityData.xModel);
			Matrix4_SetPosition(xPerEntityData.xModel, pxTransform->xPosition);
			//Matrix4_RotateX(xPerEntityData.xModel, pxTransform->xRotation[0]);
			//Matrix4_RotateY(xPerEntityData.xModel, pxTransform->xRotation[0]);
			//Matrix4_RotateZ(xPerEntityData.xModel, pxTransform->xRotation[0]);
			Matrix4_SetScale(xPerEntityData.xModel, pxTransform->xScale);

			vkCmdPushConstants(pxRenderer->xGraphicCommandBuffer, GraphicPipeline_GetPipelineLayout(pxRenderer->pxParticleGraphicPipeline), VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(xPerEntityData), &xPerEntityData);

			vkCmdDrawIndexed(pxRenderer->xGraphicCommandBuffer, pxRenderable->nIndexCount, pxParticleSystem->nParticleCount, 0, 0, 0);
		}
	}

	{
		vkCmdBindPipeline(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipeline(pxRenderer->pxDebugGraphicPipeline));

		vkCmdSetViewport(pxRenderer->xGraphicCommandBuffer, 0, 1, &xViewport);
		vkCmdSetScissor(pxRenderer->xGraphicCommandBuffer, 0, 1, &xScissor);

		VkBuffer axDefaultVertexBuffers[] = { Buffer_GetBuffer(pxRenderer->pxDebugVertexBuffer) };

		uint64_t awDefaultOffsets[] = { 0 };

		vkCmdBindDescriptorSets(pxRenderer->xGraphicCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GraphicPipeline_GetPipelineLayout(pxRenderer->pxDebugGraphicPipeline), 0, 1, &pxRenderer->xDebugGraphicDescriptorSet, 0, 0);
		vkCmdBindVertexBuffers(pxRenderer->xGraphicCommandBuffer, DEFAULT_VERTEX_BINDING_ID, 1, axDefaultVertexBuffers, awDefaultOffsets);
		vkCmdBindIndexBuffer(pxRenderer->xGraphicCommandBuffer, Buffer_GetBuffer(pxRenderer->pxDebugIndexBuffer), 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(pxRenderer->xGraphicCommandBuffer, pxRenderer->nDebugIndexCount, 1, 0, 0, 0);
	}

	vkCmdEndRenderPass(pxRenderer->xGraphicCommandBuffer);

	VK_CHECK(vkEndCommandBuffer(pxRenderer->xGraphicCommandBuffer));
}
static void Renderer_BuildComputeCommandBuffer(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	Instance_ComputeQueueWaitIdle(pxInstance);

	VkCommandBufferBeginInfo xCommandBufferBeginInfo;
	memset(&xCommandBufferBeginInfo, 0, sizeof(xCommandBufferBeginInfo));
	xCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	xCommandBufferBeginInfo.flags = 0;
	xCommandBufferBeginInfo.pInheritanceInfo = 0;

	VK_CHECK(vkBeginCommandBuffer(pxRenderer->xComputeCommandBuffer, &xCommandBufferBeginInfo));

	{
		vkCmdBindPipeline(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipeline(pxRenderer->pxParticleComputePipeline));

		for (uint32_t i = 0; i < Vector_Count(pxRenderer->pxParticleComputeEntities); ++i) {
			struct xEntity_t* pxEntity = *(struct xEntity_t**)Vector_At(pxRenderer->pxParticleComputeEntities, i);

			xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(pxEntity);

			vkCmdBindDescriptorSets(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipelineLayout(pxRenderer->pxParticleComputePipeline), 0, 1, Vector_At(pxRenderer->pxParticleComputeDescriptorSets, i), 0, 0);

			xDimensions_t xDimensions;
			memset(&xDimensions, 0, sizeof(xDimensions));
			xDimensions.fWidth = pxParticleSystem->fWidth;
			xDimensions.fHeight = pxParticleSystem->fHeight;
			xDimensions.fDepth = pxParticleSystem->fDepth;

			vkCmdPushConstants(pxRenderer->xComputeCommandBuffer, ComputePipeline_GetPipelineLayout(pxRenderer->pxParticleComputePipeline), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(xDimensions), &xDimensions);

			vkCmdDispatch(pxRenderer->xComputeCommandBuffer, (uint32_t)ceilf(pxParticleSystem->nParticleCount / 32), 1, 1);
		}
	}

	{
		vkCmdBindPipeline(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipeline(pxRenderer->pxPixelComputePipeline));

		for (uint32_t i = 0; i < Vector_Count(pxRenderer->pxPixelComputeEntities); ++i) {
			struct xEntity_t* pxEntity = *(struct xEntity_t**)Vector_At(pxRenderer->pxPixelComputeEntities, i);

			xPixelSystem_t* pxPixelSystem = Entity_GetPixelSystem(pxEntity);

			vkCmdBindDescriptorSets(pxRenderer->xComputeCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, ComputePipeline_GetPipelineLayout(pxRenderer->pxPixelComputePipeline), 0, 1, Vector_At(pxRenderer->pxPixelComputeDescriptorSets, i), 0, 0);

			xDimensions_t xDimensions;
			memset(&xDimensions, 0, sizeof(xDimensions));
			xDimensions.fWidth = pxPixelSystem->nWidth;
			xDimensions.fHeight = pxPixelSystem->nHeight;

			vkCmdPushConstants(pxRenderer->xComputeCommandBuffer, ComputePipeline_GetPipelineLayout(pxRenderer->pxPixelComputePipeline), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(xDimensions), &xDimensions);

			vkCmdDispatch(pxRenderer->xComputeCommandBuffer, (uint32_t)ceilf(pxPixelSystem->nWidth / 32), (uint32_t)ceilf(pxPixelSystem->nHeight / 32), 1);
		}
	}

	VK_CHECK(vkEndCommandBuffer(pxRenderer->xComputeCommandBuffer));
}

static void Renderer_AllocSynchronizationObjects(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	VkSemaphoreCreateInfo xSemaphoreCreateInfo;
	memset(&xSemaphoreCreateInfo, 0, sizeof(xSemaphoreCreateInfo));
	xSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->xGraphicCompleteSemaphore));
	VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->xComputeCompleteSemaphore));
	VK_CHECK(vkCreateSemaphore(Instance_GetDevice(pxInstance), &xSemaphoreCreateInfo, 0, &pxRenderer->xImageAvailableSemaphore));

	VkFenceCreateInfo xFenceCreateInfo;
	memset(&xFenceCreateInfo, 0, sizeof(xFenceCreateInfo));
	xFenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	xFenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	VK_CHECK(vkCreateFence(Instance_GetDevice(pxInstance), &xFenceCreateInfo, 0, &pxRenderer->xRenderFence));
}

static void Renderer_FreeSynchronizationObjects(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	vkDestroyFence(Instance_GetDevice(pxInstance), pxRenderer->xRenderFence, 0);

	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xImageAvailableSemaphore, 0);
	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xComputeCompleteSemaphore, 0);
	vkDestroySemaphore(Instance_GetDevice(pxInstance), pxRenderer->xGraphicCompleteSemaphore, 0);
}

static void Renderer_SetupPushConstants(struct xRenderer_t* pxRenderer) {
	pxRenderer->axDefaultGraphicPushConstantRanges[0].offset = 0;
	pxRenderer->axDefaultGraphicPushConstantRanges[0].size = sizeof(xPerEntityData_t);
	pxRenderer->axDefaultGraphicPushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	pxRenderer->axParticleGraphicPushConstantRanges[0].offset = 0;
	pxRenderer->axParticleGraphicPushConstantRanges[0].size = sizeof(xPerEntityData_t);
	pxRenderer->axParticleGraphicPushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	pxRenderer->axParticleComputePushConstantRanges[0].offset = 0;
	pxRenderer->axParticleComputePushConstantRanges[0].size = sizeof(xDimensions_t);
	pxRenderer->axParticleComputePushConstantRanges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	pxRenderer->axPixelComputePushConstantRanges[0].offset = 0;
	pxRenderer->axPixelComputePushConstantRanges[0].size = sizeof(xDimensions_t);
	pxRenderer->axPixelComputePushConstantRanges[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
}

static void Renderer_AllocDebugLineBuffers(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	Renderer_CreateDebugGraphicDescriptorSet(pxRenderer, pxInstance);
	Renderer_UpdateDebugGraphicDescriptorSet(pxRenderer, pxInstance);

	pxRenderer->pxDebugVertexBuffer = VertexBuffer_AllocCoherent(pxInstance, DEBUG_VERTEX_BUFFER_COUNT * sizeof(xDebugVertex_t));
	pxRenderer->pxDebugIndexBuffer = IndexBuffer_AllocCoherent(pxInstance, DEBUG_INDEX_BUFFER_COUNT * sizeof(uint32_t));

	pxRenderer->pxDebugVertices = Buffer_GetMappedData(pxRenderer->pxDebugVertexBuffer);
	pxRenderer->pnDebugIndices = Buffer_GetMappedData(pxRenderer->pxDebugIndexBuffer);

	pxRenderer->nDebugVertexCount = 0;
	pxRenderer->nDebugIndexCount = 0;
}

static void Renderer_FreeDebugLineBuffers(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	Buffer_Free(pxRenderer->pxDebugIndexBuffer, pxInstance);
	Buffer_Free(pxRenderer->pxDebugVertexBuffer, pxInstance);
}

struct xRenderer_t* Renderer_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	struct xRenderer_t* pxRenderer = (struct xRenderer_t*)calloc(1, sizeof(struct xRenderer_t));

	pxRenderer->pxTimeInfoBuffer = UniformBuffer_AllocCoherent(pxInstance, sizeof(xTimeInfo_t));
	pxRenderer->pxViewProjectionBuffer = UniformBuffer_AllocCoherent(pxInstance, sizeof(xViewProjection_t));

	Matrix4_Identity(pxRenderer->xViewProjection.xView);
	Matrix4_Identity(pxRenderer->xViewProjection.xProjection);

	pxRenderer->pxDefaultGraphicDescriptorSets = Vector_Alloc(sizeof(VkDescriptorSet));
	pxRenderer->pxParticleGraphicDescriptorSets = Vector_Alloc(sizeof(VkDescriptorSet));
	pxRenderer->pxParticleComputeDescriptorSets = Vector_Alloc(sizeof(VkDescriptorSet));
	pxRenderer->pxPixelComputeDescriptorSets = Vector_Alloc(sizeof(VkDescriptorSet));

	pxRenderer->pxDefaultGraphicEntities = Vector_Alloc(sizeof(struct xEntity_t*));
	pxRenderer->pxParticleGraphicEntities = Vector_Alloc(sizeof(struct xEntity_t*));
	pxRenderer->pxParticleComputeEntities = Vector_Alloc(sizeof(struct xEntity_t*));
	pxRenderer->pxPixelComputeEntities = Vector_Alloc(sizeof(struct xEntity_t*));

	Renderer_AllocDefaultGraphicDescriptorPool(pxRenderer, pxInstance);
	Renderer_AllocParticleGraphicDescriptorPool(pxRenderer, pxInstance);
	Renderer_AllocParticleComputeDescriptorPool(pxRenderer, pxInstance);
	Renderer_AllocPixelComputeDescriptorPool(pxRenderer, pxInstance);
	Renderer_AllocDebugGraphicDescriptorPool(pxRenderer, pxInstance);

	Renderer_AllocDefaultGraphicDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_AllocParticleGraphicDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_AllocParticleComputeDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_AllocPixelComputeDescriptorSetLayout(pxRenderer, pxInstance);
	Renderer_AllocDebugGraphicDescriptorSetLayout(pxRenderer, pxInstance);

	Renderer_SetupPushConstants(pxRenderer);

	Renderer_AllocDefaultGraphicPipeline(pxRenderer, pxInstance, pxSwapChain);
	Renderer_AllocParticleGraphicPipeline(pxRenderer, pxInstance, pxSwapChain);
	Renderer_AllocParticleComputePipeline(pxRenderer, pxInstance);
	Renderer_AllocPixelComputePipeline(pxRenderer, pxInstance);
	Renderer_AllocDebugGraphicPipeline(pxRenderer, pxInstance, pxSwapChain);

	Renderer_AllocGraphicCommandBuffer(pxRenderer, pxInstance);
	Renderer_AllocComputeCommandBuffer(pxRenderer, pxInstance);

	Renderer_AllocSynchronizationObjects(pxRenderer, pxInstance);

	Renderer_AllocDebugLineBuffers(pxRenderer, pxInstance);

	return pxRenderer;
}

void Renderer_Free(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance) {
	Renderer_FreeDebugLineBuffers(pxRenderer, pxInstance);
	Renderer_FreeSynchronizationObjects(pxRenderer, pxInstance);
	Renderer_FreeCommandBuffers(pxRenderer, pxInstance);
	Renderer_FreePipelines(pxRenderer, pxInstance);
	Renderer_FreeDescriptorSetLayouts(pxRenderer, pxInstance);
	Renderer_FreeDescriptorPools(pxRenderer, pxInstance);

	Vector_Free(pxRenderer->pxDefaultGraphicEntities);
	Vector_Free(pxRenderer->pxParticleGraphicEntities);
	Vector_Free(pxRenderer->pxParticleComputeEntities);
	Vector_Free(pxRenderer->pxPixelComputeEntities);

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

void Renderer_Draw(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain) {
	VK_CHECK(vkResetFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xRenderFence));

	Buffer_SetTo(pxRenderer->pxTimeInfoBuffer, &pxRenderer->xTimeInfo, sizeof(xTimeInfo_t));
	Buffer_SetTo(pxRenderer->pxViewProjectionBuffer, &pxRenderer->xViewProjection, sizeof(xViewProjection_t));

	uint32_t nImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(Instance_GetDevice(pxInstance), SwapChain_GetSwapChain(pxSwapChain), UINT64_MAX, pxRenderer->xImageAvailableSemaphore, 0, &nImageIndex));

	Renderer_BuildGraphicCommandBuffer(pxRenderer, pxInstance, pxSwapChain, nImageIndex);
	Renderer_BuildComputeCommandBuffer(pxRenderer, pxInstance);

	{
		VkSemaphore axSignalSemaphores[] = { pxRenderer->xComputeCompleteSemaphore };

		VkSubmitInfo xSubmitInfo;
		memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
		xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		xSubmitInfo.waitSemaphoreCount = 0;
		xSubmitInfo.pWaitSemaphores = 0;
		xSubmitInfo.signalSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
		xSubmitInfo.pSignalSemaphores = axSignalSemaphores;
		xSubmitInfo.pWaitDstStageMask = 0;
		xSubmitInfo.commandBufferCount = 1;
		xSubmitInfo.pCommandBuffers = &pxRenderer->xComputeCommandBuffer;

		VK_CHECK(vkQueueSubmit(Instance_GetComputeQueue(pxInstance), 1, &xSubmitInfo, 0));
	}

	{
		VkSemaphore axWaitSemaphores[] = { pxRenderer->xComputeCompleteSemaphore, pxRenderer->xImageAvailableSemaphore };
		VkSemaphore axSignalSemaphores[] = { pxRenderer->xGraphicCompleteSemaphore };
		VkPipelineStageFlags axWaitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT  };

		VkSubmitInfo xSubmitInfo;
		memset(&xSubmitInfo, 0, sizeof(xSubmitInfo));
		xSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		xSubmitInfo.waitSemaphoreCount = ARRAY_LENGTH(axWaitSemaphores);
		xSubmitInfo.pWaitSemaphores = axWaitSemaphores;
		xSubmitInfo.signalSemaphoreCount = ARRAY_LENGTH(axSignalSemaphores);
		xSubmitInfo.pSignalSemaphores = axSignalSemaphores;
		xSubmitInfo.pWaitDstStageMask = axWaitStages;
		xSubmitInfo.commandBufferCount = 1;
		xSubmitInfo.pCommandBuffers = &pxRenderer->xGraphicCommandBuffer;

		VK_CHECK(vkQueueSubmit(Instance_GetGraphicQueue(pxInstance), 1, &xSubmitInfo, pxRenderer->xRenderFence));
	}

	VK_CHECK(vkWaitForFences(Instance_GetDevice(pxInstance), 1, &pxRenderer->xRenderFence, VK_TRUE, UINT64_MAX));

	{
		VkSemaphore axWaitSemaphores[] = { pxRenderer->xGraphicCompleteSemaphore };
		VkSwapchainKHR axSwapChains[] = { SwapChain_GetSwapChain(pxSwapChain) };

		VkPresentInfoKHR xPresentInfo;
		memset(&xPresentInfo, 0, sizeof(xPresentInfo));
		xPresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		xPresentInfo.waitSemaphoreCount = ARRAY_LENGTH(axWaitSemaphores);
		xPresentInfo.pWaitSemaphores = axWaitSemaphores;
		xPresentInfo.swapchainCount = ARRAY_LENGTH(axSwapChains);
		xPresentInfo.pSwapchains = axSwapChains;
		xPresentInfo.pImageIndices = &nImageIndex;

		VK_CHECK(vkQueuePresentKHR(Instance_GetPresentQueue(pxInstance), &xPresentInfo));
	}

	pxRenderer->nDebugVertexCount = 0;
	pxRenderer->nDebugIndexCount = 0;
}
void Renderer_DrawDebugLine(struct xRenderer_t* pxRenderer, xVec3_t xPositionA, xVec3_t xPositionB, xVec4_t xColorA, xVec4_t xColorB) {
	Vector3_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 0].xPosition, xPositionA[0], xPositionA[1], xPositionA[2]);
	Vector3_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 1].xPosition, xPositionB[0], xPositionB[1], xPositionB[2]);

	Vector4_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 0].xColor, xColorA[0], xColorA[1], xColorA[2], xColorA[3]);
	Vector4_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 1].xColor, xColorB[0], xColorB[1], xColorB[2], xColorB[3]);

	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 0] = pxRenderer->nDebugVertexCount + 0;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 1] = pxRenderer->nDebugVertexCount + 1;

	pxRenderer->nDebugVertexCount += 2;
	pxRenderer->nDebugIndexCount += 2;
}
void Renderer_DrawDebugBox(struct xRenderer_t* pxRenderer, xVec3_t xPosition, xVec3_t xSize, xVec4_t xColor, xVec4_t xRotation) {
	xVec3_t xHs;

	Vector3_DivScalar(xSize, 2.0F, xHs);

	xVec3_t xBlb = { -xHs[0], -xHs[1], -xHs[2] };
	xVec3_t xBrb = {  xHs[0], -xHs[1], -xHs[2] };
	xVec3_t xTlb = { -xHs[0],  xHs[1], -xHs[2] };
	xVec3_t xTrb = {  xHs[0],  xHs[1], -xHs[2] };

	xVec3_t xBlf = { -xHs[0], -xHs[1],  xHs[2] };
	xVec3_t xBrf = {  xHs[0], -xHs[1],  xHs[2] };
	xVec3_t xTlf = { -xHs[0],  xHs[1],  xHs[2] };
	xVec3_t xTrf = {  xHs[0],  xHs[1],  xHs[2] };

	Vector3_Add(xBlb, xPosition, xBlb);
	Vector3_Add(xBrb, xPosition, xBrb);
	Vector3_Add(xTlb, xPosition, xTlb);
	Vector3_Add(xTrb, xPosition, xTrb);

	Vector3_Add(xBlf, xPosition, xBlf);
	Vector3_Add(xBrf, xPosition, xBrf);
	Vector3_Add(xTlf, xPosition, xTlf);
	Vector3_Add(xTrf, xPosition, xTrf);

	Vector3_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 0].xPosition, xBlb[0], xBlb[1], xBlb[2]);
	Vector3_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 1].xPosition, xBrb[0], xBrb[1], xBrb[2]);
	Vector3_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 2].xPosition, xTlb[0], xTlb[1], xTlb[2]);
	Vector3_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 3].xPosition, xTrb[0], xTrb[1], xTrb[2]);

	Vector3_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 4].xPosition, xBlf[0], xBlf[1], xBlf[2]);
	Vector3_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 5].xPosition, xBrf[0], xBrf[1], xBrf[2]);
	Vector3_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 6].xPosition, xTlf[0], xTlf[1], xTlf[2]);
	Vector3_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 7].xPosition, xTrf[0], xTrf[1], xTrf[2]);

	Vector4_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 0].xColor, xColor[0], xColor[1], xColor[2], xColor[3]);
	Vector4_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 1].xColor, xColor[0], xColor[1], xColor[2], xColor[3]);
	Vector4_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 2].xColor, xColor[0], xColor[1], xColor[2], xColor[3]);
	Vector4_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 3].xColor, xColor[0], xColor[1], xColor[2], xColor[3]);

	Vector4_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 4].xColor, xColor[0], xColor[1], xColor[2], xColor[3]);
	Vector4_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 5].xColor, xColor[0], xColor[1], xColor[2], xColor[3]);
	Vector4_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 6].xColor, xColor[0], xColor[1], xColor[2], xColor[3]);
	Vector4_Set(pxRenderer->pxDebugVertices[pxRenderer->nDebugVertexCount + 7].xColor, xColor[0], xColor[1], xColor[2], xColor[3]);

	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 0] = pxRenderer->nDebugVertexCount + 0;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 1] = pxRenderer->nDebugVertexCount + 1;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 2] = pxRenderer->nDebugVertexCount + 1;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 3] = pxRenderer->nDebugVertexCount + 3;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 4] = pxRenderer->nDebugVertexCount + 3;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 5] = pxRenderer->nDebugVertexCount + 2;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 6] = pxRenderer->nDebugVertexCount + 2;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 7] = pxRenderer->nDebugVertexCount + 0;

	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 8] = pxRenderer->nDebugVertexCount + 4;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 9] = pxRenderer->nDebugVertexCount + 5;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 10] = pxRenderer->nDebugVertexCount + 5;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 11] = pxRenderer->nDebugVertexCount + 7;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 12] = pxRenderer->nDebugVertexCount + 7;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 13] = pxRenderer->nDebugVertexCount + 6;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 14] = pxRenderer->nDebugVertexCount + 6;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 15] = pxRenderer->nDebugVertexCount + 4;

	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 16] = pxRenderer->nDebugVertexCount + 0;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 17] = pxRenderer->nDebugVertexCount + 4;

	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 18] = pxRenderer->nDebugVertexCount + 1;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 19] = pxRenderer->nDebugVertexCount + 5;

	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 20] = pxRenderer->nDebugVertexCount + 2;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 21] = pxRenderer->nDebugVertexCount + 6;

	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 22] = pxRenderer->nDebugVertexCount + 3;
	pxRenderer->pnDebugIndices[pxRenderer->nDebugIndexCount + 23] = pxRenderer->nDebugVertexCount + 7;

	pxRenderer->nDebugVertexCount += 8;
	pxRenderer->nDebugIndexCount += 24;
}

void Renderer_CommitEntities(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xList_t* pxEntities) {
	uint32_t nDefaultGraphicDescriptorCount = 0;
	uint32_t nParticleGraphicDescriptorCount = 0;
	uint32_t nParticleComputeDescriptorCount = 0;
	uint32_t nPixelComputeDescriptorCount = 0;

	{
		void* pEntityIter = List_Begin(pxEntities);
		while (pEntityIter) {
			struct xEntity_t* pxEntity = *(struct xEntity_t**)List_Value(pEntityIter);

			if (Entity_HasComponents(pxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT)) {
				nDefaultGraphicDescriptorCount++;
			}

			if (Entity_HasComponents(pxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT | COMPONENT_PARTICLESYSTEM_BIT)) {
				nParticleGraphicDescriptorCount++;
			}

			if (Entity_HasComponents(pxEntity, COMPONENT_PARTICLESYSTEM_BIT)) {
				nParticleComputeDescriptorCount++;
			}

			if (Entity_HasComponents(pxEntity, COMPONENT_PIXELSYSTEM_BIT)) {
				nPixelComputeDescriptorCount++;
			}

			pEntityIter = List_Next(pEntityIter);
		}
	}

	if (nDefaultGraphicDescriptorCount == 0) nDefaultGraphicDescriptorCount = 1;
	if (nParticleGraphicDescriptorCount == 0) nParticleGraphicDescriptorCount = 1;
	if (nParticleComputeDescriptorCount == 0) nParticleComputeDescriptorCount = 1;
	if (nPixelComputeDescriptorCount == 0) nPixelComputeDescriptorCount = 1;

#ifdef DEBUG
	printf("Using (%u/%u) DefaultGraphicDescriptorSets\n", nDefaultGraphicDescriptorCount, INITIAL_DEFAULT_GRAPHIC_POOL_COUNT);
	printf("Using (%u/%u) ParticleGraphicDescriptorSets\n", nParticleGraphicDescriptorCount, INITIAL_PARTICLE_GRAPHIC_POOL_COUNT);
	printf("Using (%u/%u) ParticleComputeDescriptorSets\n", nParticleComputeDescriptorCount, INITIAL_PARTICLE_COMPUTE_POOL_COUNT);
	printf("Using (%u/%u) PixelComputeDescriptorSets\n", nPixelComputeDescriptorCount, INITIAL_PIXEL_COMPUTE_POOL_COUNT);
#endif

	Vector_Resize(pxRenderer->pxDefaultGraphicDescriptorSets, nDefaultGraphicDescriptorCount);
	Vector_Resize(pxRenderer->pxParticleGraphicDescriptorSets, nParticleGraphicDescriptorCount);
	Vector_Resize(pxRenderer->pxParticleComputeDescriptorSets, nParticleComputeDescriptorCount);
	Vector_Resize(pxRenderer->pxPixelComputeDescriptorSets, nPixelComputeDescriptorCount);

	Renderer_CreateDefaultGraphicDescriptorSets(pxRenderer, pxInstance, nDefaultGraphicDescriptorCount);
	Renderer_CreateParticleGraphicDescriptorSets(pxRenderer, pxInstance, nParticleGraphicDescriptorCount);
	Renderer_CreateParticleComputeDescriptorSets(pxRenderer, pxInstance, nParticleComputeDescriptorCount);
	Renderer_CreatePixelComputeDescriptorSets(pxRenderer, pxInstance, nPixelComputeDescriptorCount);

	Vector_Clear(pxRenderer->pxDefaultGraphicEntities);
	Vector_Clear(pxRenderer->pxParticleGraphicEntities);
	Vector_Clear(pxRenderer->pxParticleComputeEntities);
	Vector_Clear(pxRenderer->pxPixelComputeEntities);

	uint32_t nDefaultGraphicDescriptorIndex = 0;
	uint32_t nParticleGraphicDescriptorIndex = 0;
	uint32_t nParticleComputeDescriptorIndex = 0;
	uint32_t nPixelComputeDescriptorIndex = 0;

	{
		void* pEntityIter = List_Begin(pxEntities);
		while (pEntityIter) {
			struct xEntity_t* pxEntity = *(struct xEntity_t**)List_Value(pEntityIter);
	
			if (Entity_HasComponents(pxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT)) {
				xRenderable_t* pxRenderable = Entity_GetRenderable(pxEntity);

				Renderer_UpdateDefaultGraphicDescriptorSet(pxRenderer, pxInstance, nDefaultGraphicDescriptorIndex, pxRenderable);

				Vector_Push(pxRenderer->pxDefaultGraphicEntities, &pxEntity);

				nDefaultGraphicDescriptorIndex++;
			}

			if (Entity_HasComponents(pxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_RENDERABLE_BIT | COMPONENT_PARTICLESYSTEM_BIT)) {
				xRenderable_t* pxRenderable = Entity_GetRenderable(pxEntity);

				Renderer_UpdateParticleGraphicDescriptorSet(pxRenderer, pxInstance, nParticleGraphicDescriptorIndex, pxRenderable);

				Vector_Push(pxRenderer->pxParticleGraphicEntities, &pxEntity);

				nParticleGraphicDescriptorIndex++;
			}
	
			if (Entity_HasComponents(pxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_PARTICLESYSTEM_BIT)) {
				xParticleSystem_t* pxParticleSystem = Entity_GetParticleSystem(pxEntity);

				Renderer_UpdateParticleComputeDescriptorSet(pxRenderer, pxInstance, nParticleComputeDescriptorIndex, pxParticleSystem);

				Vector_Push(pxRenderer->pxParticleComputeEntities, &pxEntity);

				nParticleComputeDescriptorIndex++;
			}

			if (Entity_HasComponents(pxEntity, COMPONENT_TRANSFORM_BIT | COMPONENT_PIXELSYSTEM_BIT)) {
				xPixelSystem_t* pxPixelSystem = Entity_GetPixelSystem(pxEntity);

				Renderer_UpdatePixelComputeDescriptorSet(pxRenderer, pxInstance, nPixelComputeDescriptorIndex, pxPixelSystem);

				Vector_Push(pxRenderer->pxPixelComputeEntities, &pxEntity);

				nPixelComputeDescriptorIndex++;
			}

			pEntityIter = List_Next(pEntityIter);
		}
	}
}
