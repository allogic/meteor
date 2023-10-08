#ifndef METEOR_VULKAN_RENDERER_H
#define METEOR_VULKAN_RENDERER_H

#include <stdint.h>

#include <vulkan/uniform.h>
#include <vulkan/graphicpipeline.h>
#include <vulkan/computepipeline.h>
#include <vulkan/vulkan.h>

#define DEFAULT_VERTEX_BINDING_ID 0
#define INSTANCE_VERTEX_BINDING_ID 1

struct xInstance_t;
struct xSwapChain_t;
struct xRenderer_t;
struct xList_t;

typedef struct xDescriptorValue_t {
    union xDescriptorType_t {

    } xType;
    uint32_t
};

struct xRenderer_t* Renderer_Alloc(struct xInstance_t* pxInstance);
void Renderer_Free(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance);

xTimeInfo_t* Renderer_GetTimeInfo(struct xRenderer_t* pxRenderer);
xViewProjection_t* Renderer_GetViewProjection(struct xRenderer_t* pxRenderer);

void Renderer_PushGraphicPipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, const char* pcName, xMaterialMapHandler_t pMaterialMapHandler, VkPrimitiveTopology xPrimitiveTopology, uint32_t nPoolCount, const char* pcVertexFilePath, const char* pcFragmentFilePath, VkDescriptorSetLayoutBinding* pxDescriptorSetLayoutBindings, uint32_t nDescriptorSetLayoutBindingCount, VkVertexInputBindingDescription* pxVertexInputBindingDescriptions, uint32_t nVertexInputBindingDescriptionCount, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions, uint32_t nVertexInputAttributeDescriptionCount, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantRangeCount);
void Renderer_PushComputePipeline(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, const char* pcName, xMaterialMapHandler_t pMaterialMapHandler, uint32_t nPoolCount, const char* pcComputeFilePath, VkDescriptorSetLayoutBinding* pxDescriptorSetLayoutBindings, uint32_t nDescriptorSetLayoutBindingCount, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantRangeCount);

void Renderer_Draw(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain);
void Renderer_DrawDebugLine(struct xRenderer_t* pxRenderer, xVec3_t xVecA, xVec3_t xVecB, xVec4_t xColorA, xVec4_t xColorB);
void Renderer_DrawDebugBox(struct xRenderer_t* pxRenderer, xVec3_t xPosition, xVec3_t xSize, xVec4_t xColor, xVec4_t xRotation);

void Renderer_CommitEntities(struct xRenderer_t* pxRenderer, struct xInstance_t* pxInstance, struct xList_t* pxEntities);

#endif
