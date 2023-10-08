#ifndef METEOR_VULKAN_COMPUTEPIPELINE_H
#define METEOR_VULKAN_COMPUTEPIPELINE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xComputePipeline_t;
struct xList_t;
struct xEntity_t;

typedef void(*xMaterialMapHandler_t)(void*);

struct xComputePipeline_t* ComputePipeline_Alloc(struct xInstance_t* pxInstance, const char* pcName, xMaterialMapHandler_t pMaterialMapHandler, uint32_t nPoolCount, const char* pcComputeFilePath, VkDescriptorSetLayoutBinding* pxDescriptorSetLayoutBindings, uint32_t nDescriptorSetLayoutBindingCount, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantRangeCount);
void ComputePipeline_Free(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance);

const char* ComputePipeline_GetName(struct xComputePipeline_t* pxComputePipeline);
VkDescriptorPool ComputePipeline_GetDescriptorPool(struct xComputePipeline_t* pxComputePipeline);
VkDescriptorSetLayout ComputePipeline_GetDescriptorSetLayout(struct xComputePipeline_t* pxComputePipeline);
VkPipelineLayout ComputePipeline_GetPipelineLayout(struct xComputePipeline_t* pxComputePipeline);
VkPipeline ComputePipeline_GetPipeline(struct xComputePipeline_t* pxComputePipeline);
struct xList_t* ComputePipeline_GetEntities(struct xComputePipeline_t* pxComputePipeline);

void ComputePipeline_CreateDescriptorSets(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance);
void ComputePipeline_UpdateDescriptorSets(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance);

void ComputePipeline_PushEntity(struct xComputePipeline_t* pxComputePipeline, struct xEntity_t* pxEntity);

#endif
