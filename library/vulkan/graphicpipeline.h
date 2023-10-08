#ifndef METEOR_VULKAN_GRAPHICPIPELINE_H
#define METEOR_VULKAN_GRAPHICPIPELINE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xSwapChain_t;
struct xGraphicPipeline_t;
struct xList_t;
struct xEntity_t;

typedef void(*xMaterialMapHandler_t)(void*);

struct xGraphicPipeline_t* GraphicPipeline_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, const char* pcName, xMaterialMapHandler_t pMaterialMapHandler, VkPrimitiveTopology xPrimitiveTopology, uint32_t nPoolCount, const char* pcVertexFilePath, const char* pcFragmentFilePath, VkDescriptorSetLayoutBinding* pxDescriptorSetLayoutBindings, uint32_t nDescriptorSetLayoutBindingCount, VkVertexInputBindingDescription* pxVertexInputBindingDescriptions, uint32_t nVertexInputBindingDescriptionCount, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions, uint32_t nVertexInputAttributeDescriptionCount, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantRangeCount);
void GraphicPipeline_Free(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance);

const char* GraphicPipeline_GetName(struct xGraphicPipeline_t* pxGraphicPipeline);
VkDescriptorPool GraphicPipeline_GetDescriptorPool(struct xGraphicPipeline_t* pxGraphicPipeline);
VkDescriptorSetLayout GraphicPipeline_GetDescriptorSetLayout(struct xGraphicPipeline_t* pxGraphicPipeline);
VkPipelineLayout GraphicPipeline_GetPipelineLayout(struct xGraphicPipeline_t* pxGraphicPipeline);
VkPipeline GraphicPipeline_GetPipeline(struct xGraphicPipeline_t* pxGraphicPipeline);
struct xList_t* GraphicPipeline_GetEntities(struct xGraphicPipeline_t* pxGraphicPipeline);

void GraphicPipeline_CreateDescriptorSets(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance);
void GraphicPipeline_UpdateDescriptorSets(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance);

void GraphicPipeline_PushEntity(struct xGraphicPipeline_t* pxGraphicPipeline, struct xEntity_t* pxEntity);

#endif
