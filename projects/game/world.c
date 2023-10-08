#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <common/macros.h>
#include <common/timer.h>

#include <container/vector.h>

#include <ecs/component.h>
#include <ecs/entity.h>

#include <math/vector.h>

#include <random/xorshift128.h>

#include <vulkan/instance.h>
#include <vulkan/vertex.h>
#include <vulkan/buffer.h>
#include <vulkan/buffervariance.h>
#include <vulkan/image.h>
#include <vulkan/imagevariance.h>
#include <vulkan/vulkan.h>
#include <vulkan/pushconstants.h>
#include <vulkan/renderer.h>

#include <game/scene.h>
#include <game/storage.h>
#include <game/world.h>
#include <game/pushconstants.h>

struct xWorld_t {
	struct xBuffer_t* pxSharedQuadVertexBuffer;
	struct xBuffer_t* pxSharedQuadIndexBuffer;
	struct xEntity_t* apTrees[4];
};

static xDefaultVertex_t s_axVertices[4] = {
	{ { -0.5F, -0.5F, 0.0F }, { 0.0F, 1.0F }, { 0.0F, 0.0F, 0.0F, 1.0F } },
	{ { -0.5F,  0.5F, 0.0F }, { 0.0F, 0.0F }, { 0.0F, 0.0F, 0.0F, 1.0F } },
	{ {  0.5F, -0.5F, 0.0F }, { 1.0F, 1.0F }, { 0.0F, 0.0F, 0.0F, 1.0F } },
	{ {  0.5F,  0.5F, 0.0F }, { 1.0F, 0.0F }, { 0.0F, 0.0F, 0.0F, 1.0F } },
};

static uint32_t s_anIndices[6] = {
	0, 2, 1, 2, 3, 1,
};

struct xTreeMaterial_t {
	struct xImage_t* pxColorImage;
};

static void World_TreeDescriptorMapHandler(void* pMaterial, struct xVector_t* pxDescriptorInfos) {
	struct xTreeMaterial_t* pxTreeMaterial = (struct xTreeMaterial_t*)pMaterial;

	//pxRenderer->pxViewProjectionBuffer
	//pxRenderer->pxTimeInfoBuffer
	//pxRenderable->pxColorImage

	{
		struct xDescriptorInfo_t {
			eImageInfo,
			Buffer_GetBuffer(pxRenderer->pxViewProjectionBuffer),
			Buffer_GetSize(pxRenderer->pxViewProjectionBuffer)
		} xDescriptorInfo;

		Vector_Push(pxDescriptorInfos, );
	}

	{
		struct xDescriptorInfo_t {
			eImageInfo,
			Buffer_GetBuffer(pxRenderer->pxViewProjectionBuffer),
			Buffer_GetSize(pxRenderer->pxViewProjectionBuffer)
		} xDescriptorInfo;

		Vector_Push(pxDescriptorInfos, );
	}

	//VkDescriptorBufferInfo xViewProjectionDescriptorBufferInfo;
	//memset(&xViewProjectionDescriptorBufferInfo, 0, sizeof(xViewProjectionDescriptorBufferInfo));
	//xViewProjectionDescriptorBufferInfo.offset = 0;
	//xViewProjectionDescriptorBufferInfo.buffer = ;
	//xViewProjectionDescriptorBufferInfo.range = ;
//
	//VkDescriptorBufferInfo xTimeInfoDescriptorBufferInfo;
	//memset(&xTimeInfoDescriptorBufferInfo, 0, sizeof(xTimeInfoDescriptorBufferInfo));
	//xTimeInfoDescriptorBufferInfo.offset = 0;
	//xTimeInfoDescriptorBufferInfo.buffer = Buffer_GetBuffer(pxRenderer->pxTimeInfoBuffer);
	//xTimeInfoDescriptorBufferInfo.range = Buffer_GetSize(pxRenderer->pxTimeInfoBuffer);
//
	//VkDescriptorImageInfo xCombinedImageSamplerDescriptorImageInfo;
	//memset(&xCombinedImageSamplerDescriptorImageInfo, 0, sizeof(xCombinedImageSamplerDescriptorImageInfo));
	//xCombinedImageSamplerDescriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	//xCombinedImageSamplerDescriptorImageInfo.imageView = Image_GetImageView(pxRenderable->pxColorImage);
	//xCombinedImageSamplerDescriptorImageInfo.sampler = Image_GetSampler(pxRenderable->pxColorImage);
}

static void World_AllocGraphicPipelines(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xRenderer_t* pxRenderer) {
	{
		VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[] = {
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0 },
			{ 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0 },
		};

		VkVertexInputBindingDescription axVertexInputBindingDescriptions[] = {
			{ DEFAULT_VERTEX_BINDING_ID, sizeof(xDefaultVertex_t), VK_VERTEX_INPUT_RATE_VERTEX },
		};

		VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[] = {
			{ 0, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32B32_SFLOAT, 0 },
			{ 1, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32_SFLOAT, sizeof(xVec3_t) },
			{ 2, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(xVec3_t) + sizeof(xVec2_t) },
		};

		VkPushConstantRange axPushConstantRanges[] = {
			{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(xPerEntityData_t) },
		};

		Renderer_PushGraphicPipeline(
			pxRenderer, pxInstance, pxSwapChain,
			"tree", World_TreeDescriptorMapHandler, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 1024,
			"shaders/tree.vert.spv", "shaders/tree.frag.spv",
			axDescriptorSetLayoutBindings, ARRAY_LENGTH(axDescriptorSetLayoutBindings),
			axVertexInputBindingDescriptions, ARRAY_LENGTH(axVertexInputBindingDescriptions),
			axVertexInputAttributeDescriptions, ARRAY_LENGTH(axVertexInputAttributeDescriptions),
			axPushConstantRanges, ARRAY_LENGTH(axPushConstantRanges));
	}

	{
		VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[] = {
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
			{ 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, 0 },
		};

		VkVertexInputBindingDescription axVertexInputBindingDescriptions[] = {
			{ DEFAULT_VERTEX_BINDING_ID, sizeof(xDefaultVertex_t), VK_VERTEX_INPUT_RATE_VERTEX },
			{ INSTANCE_VERTEX_BINDING_ID, sizeof(xParticle_t), VK_VERTEX_INPUT_RATE_INSTANCE },
		};

		VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[] = {
			{ 0, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32B32_SFLOAT, 0 },
			{ 1, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32_SFLOAT, sizeof(xVec3_t) },
			{ 2, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(xVec3_t) + sizeof(xVec2_t) },
			{ 3, INSTANCE_VERTEX_BINDING_ID, VK_FORMAT_R32G32B32A32_SFLOAT, 0 },
			{ 4, INSTANCE_VERTEX_BINDING_ID, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(xVec4_t) },
		};

		VkPushConstantRange axPushConstantRanges[] = {
			{ VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(xPerEntityData_t) },
		};

		Renderer_PushGraphicPipeline(
			pxRenderer, pxInstance, pxSwapChain,
			"particle", 0, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 1024,
			"shaders/particle.vert.spv", "shaders/particle.frag.spv",
			axDescriptorSetLayoutBindings, ARRAY_LENGTH(axDescriptorSetLayoutBindings),
			axVertexInputBindingDescriptions, ARRAY_LENGTH(axVertexInputBindingDescriptions),
			axVertexInputAttributeDescriptions, ARRAY_LENGTH(axVertexInputAttributeDescriptions),
			axPushConstantRanges, ARRAY_LENGTH(axPushConstantRanges));
	}

	{
		VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[] = {
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
		};

		VkVertexInputBindingDescription axVertexInputBindingDescriptions[] = {
			{ DEFAULT_VERTEX_BINDING_ID, sizeof(xInterfaceVertex_t), VK_VERTEX_INPUT_RATE_VERTEX },
		};

		VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[] = {
			{ 0, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32B32_SFLOAT, 0 },
			{ 1, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32_SFLOAT, sizeof(xVec3_t) },
			{ 2, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(xVec3_t) + sizeof(xVec2_t) },
		};

		Renderer_PushGraphicPipeline(
			pxRenderer, pxInstance, pxSwapChain,
			"interface", 0, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 1024,
			"shaders/interface.vert.spv", "shaders/interface.frag.spv",
			axDescriptorSetLayoutBindings, ARRAY_LENGTH(axDescriptorSetLayoutBindings),
			axVertexInputBindingDescriptions, ARRAY_LENGTH(axVertexInputBindingDescriptions),
			axVertexInputAttributeDescriptions, ARRAY_LENGTH(axVertexInputAttributeDescriptions),
			0, 0);
	}

	{
		VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[] = {
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, 0 },
		};

		VkVertexInputBindingDescription axVertexInputBindingDescriptions[] = {
			{ DEFAULT_VERTEX_BINDING_ID, sizeof(xDebugVertex_t), VK_VERTEX_INPUT_RATE_VERTEX },
		};

		VkVertexInputAttributeDescription axVertexInputAttributeDescriptions[] = {
			{ 0, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32B32_SFLOAT, 0 },
			{ 1, DEFAULT_VERTEX_BINDING_ID, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(xVec3_t) },
		};

		Renderer_PushGraphicPipeline(
			pxRenderer, pxInstance, pxSwapChain,
			"debug", 0, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 1024,
			"shaders/debug.vert.spv", "shaders/debug.frag.spv",
			axDescriptorSetLayoutBindings, ARRAY_LENGTH(axDescriptorSetLayoutBindings),
			axVertexInputBindingDescriptions, ARRAY_LENGTH(axVertexInputBindingDescriptions),
			axVertexInputAttributeDescriptions, ARRAY_LENGTH(axVertexInputAttributeDescriptions),
			0, 0);
	}
}
static void World_AllocComputePipelines(struct xInstance_t* pxInstance, struct xRenderer_t* pxRenderer) {
	{
		VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[] = {
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
		};

		VkPushConstantRange axPushConstantRanges[] = {
			{ VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(xDimensions_t) },
		};

		Renderer_PushComputePipeline(
			pxRenderer, pxInstance,
			"particle", 0, 1024,
			"shaders/particle.comp.spv",
			axDescriptorSetLayoutBindings, ARRAY_LENGTH(axDescriptorSetLayoutBindings),
			axPushConstantRanges, ARRAY_LENGTH(axPushConstantRanges));
	}

	{
		VkDescriptorSetLayoutBinding axDescriptorSetLayoutBindings[] = {
			{ 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 2, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 3, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 4, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 5, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 6, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 7, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 8, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 9, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 10, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 11, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
			{ 12, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, VK_SHADER_STAGE_COMPUTE_BIT, 0 },
		};

		VkPushConstantRange axPushConstantRanges[] = {
			{ VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(xDimensions_t) },
		};

		Renderer_PushComputePipeline(
			pxRenderer, pxInstance,
			"pixel", 0, 1024,
			"shaders/pixel.comp.spv",
			axDescriptorSetLayoutBindings, ARRAY_LENGTH(axDescriptorSetLayoutBindings),
			axPushConstantRanges, ARRAY_LENGTH(axPushConstantRanges));
	}
}

static void World_AllocTrees(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	pxWorld->apTrees[0] = Scene_AllocEntity(pxScene, "tree_a", 0);
	pxWorld->apTrees[1] = Scene_AllocEntity(pxScene, "tree_b", 0);
	pxWorld->apTrees[2] = Scene_AllocEntity(pxScene, "tree_c", 0);
	pxWorld->apTrees[3] = Scene_AllocEntity(pxScene, "tree_d", 0);

	Entity_SetTransform(pxWorld->apTrees[0], 0);
	Entity_SetTransform(pxWorld->apTrees[1], 0);
	Entity_SetTransform(pxWorld->apTrees[2], 0);
	Entity_SetTransform(pxWorld->apTrees[3], 0);

	Entity_SetRenderable(pxWorld->apTrees[0], 0);
	Entity_SetRenderable(pxWorld->apTrees[1], 0);
	Entity_SetRenderable(pxWorld->apTrees[2], 0);
	Entity_SetRenderable(pxWorld->apTrees[3], 0);

	xTransform_t* pxTransformA = Entity_GetTransform(pxWorld->apTrees[0]);
	xTransform_t* pxTransformB = Entity_GetTransform(pxWorld->apTrees[1]);
	xTransform_t* pxTransformC = Entity_GetTransform(pxWorld->apTrees[2]);
	xTransform_t* pxTransformD = Entity_GetTransform(pxWorld->apTrees[3]);

	xRenderable_t* pxRenderableA = Entity_GetRenderable(pxWorld->apTrees[0]);
	xRenderable_t* pxRenderableB = Entity_GetRenderable(pxWorld->apTrees[1]);
	xRenderable_t* pxRenderableC = Entity_GetRenderable(pxWorld->apTrees[2]);
	xRenderable_t* pxRenderableD = Entity_GetRenderable(pxWorld->apTrees[3]);

	Vector3_Set(pxTransformA->xPosition, -7.5F, 0.0F, 0.0F);
	Vector3_Set(pxTransformB->xPosition, -2.5F, 0.0F, 0.0F);
	Vector3_Set(pxTransformC->xPosition, 2.5F, 0.0F, 0.0F);
	Vector3_Set(pxTransformD->xPosition, 7.5F, 0.0F, 0.0F);

	Vector3_Set(pxTransformA->xScale, 4.0F, 4.0F, 1.0F);
	Vector3_Set(pxTransformB->xScale, 4.0F, 4.0F, 1.0F);
	Vector3_Set(pxTransformC->xScale, 4.0F, 4.0F, 1.0F);
	Vector3_Set(pxTransformD->xScale, 4.0F, 4.0F, 1.0F);

	xTreeMaterial_t* pxTreeMaterialA = calloc(1, sizeof(xTreeMaterial_t));
	xTreeMaterial_t* pxTreeMaterialB = calloc(1, sizeof(xTreeMaterial_t));
	xTreeMaterial_t* pxTreeMaterialC = calloc(1, sizeof(xTreeMaterial_t));
	xTreeMaterial_t* pxTreeMaterialD = calloc(1, sizeof(xTreeMaterial_t));

	pxTreeMaterialA->pxColorImage = StandardImage_Alloc(pxInstance, "assets/trees/tree_a.bmp");
	pxTreeMaterialB->pxColorImage = StandardImage_Alloc(pxInstance, "assets/trees/tree_b.bmp");
	pxTreeMaterialC->pxColorImage = StandardImage_Alloc(pxInstance, "assets/trees/tree_c.bmp");
	pxTreeMaterialD->pxColorImage = StandardImage_Alloc(pxInstance, "assets/trees/tree_d.bmp");

	memcpy(pxRenderableA->acPipelineName, "tree", 4);
	pxRenderableA->bSharedVertexBuffer = true;
	pxRenderableA->bSharedIndexBuffer = true;
	pxRenderableA->bSharedMaterial = false;
	pxRenderableA->pxVertexBuffer = pxWorld->pxSharedQuadVertexBuffer;
	pxRenderableA->pxIndexBuffer = pxWorld->pxSharedQuadIndexBuffer;
	pxRenderableA->nIndexCount = 6;
	pxRenderableA->pMaterial = pxTreeMaterialA;

	memcpy(pxRenderableB->acPipelineName, "tree", 4);
	pxRenderableB->bSharedVertexBuffer = true;
	pxRenderableB->bSharedIndexBuffer = true;
	pxRenderableB->bSharedMaterial = false;
	pxRenderableB->pxVertexBuffer = pxWorld->pxSharedQuadVertexBuffer;
	pxRenderableB->pxIndexBuffer = pxWorld->pxSharedQuadIndexBuffer;
	pxRenderableB->nIndexCount = 6;
	pxRenderableB->pMaterial = pxTreeMaterialB;

	memcpy(pxRenderableC->acPipelineName, "tree", 4);
	pxRenderableC->bSharedVertexBuffer = true;
	pxRenderableC->bSharedIndexBuffer = true;
	pxRenderableC->bSharedMaterial = false;
	pxRenderableC->pxVertexBuffer = pxWorld->pxSharedQuadVertexBuffer;
	pxRenderableC->pxIndexBuffer = pxWorld->pxSharedQuadIndexBuffer;
	pxRenderableC->nIndexCount = 6;
	pxRenderableC->pMaterial = pxTreeMaterialC;

	memcpy(pxRenderableD->acPipelineName, "tree", 4);
	pxRenderableD->bSharedVertexBuffer = true;
	pxRenderableD->bSharedIndexBuffer = true;
	pxRenderableD->bSharedMaterial = false;
	pxRenderableD->pxVertexBuffer = pxWorld->pxSharedQuadVertexBuffer;
	pxRenderableD->pxIndexBuffer = pxWorld->pxSharedQuadIndexBuffer;
	pxRenderableD->nIndexCount = 6;
	pxRenderableD->pMaterial = pxTreeMaterialD;
}
static void World_FreeTrees(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	for (uint32_t i = 0; i < ARRAY_LENGTH(pxWorld->apTrees); ++i) {
		Scene_FreeEntity(pxScene, pxInstance, pxWorld->apTrees[i]);
	}
}

struct xWorld_t* World_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, struct xScene_t* pxScene, struct xRenderer_t* pxRenderer) {
	struct xWorld_t* pxWorld = (struct xWorld_t*)calloc(1, sizeof(struct xWorld_t));

	pxWorld->pxSharedQuadVertexBuffer = VertexBuffer_AllocDevice(pxInstance, s_axVertices, sizeof(xDefaultVertex_t) * 4);
	pxWorld->pxSharedQuadIndexBuffer = IndexBuffer_AllocDevice(pxInstance, s_anIndices, sizeof(uint32_t) * 6);

	World_AllocGraphicPipelines(pxInstance, pxSwapChain, pxRenderer);
	World_AllocComputePipelines(pxInstance, pxRenderer);

	World_AllocTrees(pxWorld, pxInstance, pxScene);

	Scene_Commit(pxScene, pxInstance);

	return pxWorld;
}

void World_Free(struct xWorld_t* pxWorld, struct xInstance_t* pxInstance, struct xScene_t* pxScene) {
	World_FreeTrees(pxWorld, pxInstance, pxScene);

	Buffer_Free(pxWorld->pxSharedQuadIndexBuffer, pxInstance);
	Buffer_Free(pxWorld->pxSharedQuadVertexBuffer, pxInstance);

	free(pxWorld);
}

void World_Update(struct xWorld_t* pxWorld, struct xRenderer_t* pxRenderer, struct xTimer_t* pxTimer) {
	UNUSED(pxWorld);
	UNUSED(pxRenderer);
	UNUSED(pxTimer);
}
