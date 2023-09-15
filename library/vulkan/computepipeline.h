#ifndef METEOR_VULKAN_COMPUTEPIPELINE_H
#define METEOR_VULKAN_COMPUTEPIPELINE_H

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xComputePipeline_t;
struct xVector_t;

struct xComputePipeline_t* ComputePipeline_Alloc(struct xInstance_t* pxInstance, VkShaderModule xCompModule, struct xVector_t* pxDescriptorSetLayouts);
void ComputePipeline_Free(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance);

VkPipelineLayout ComputePipeline_GetPipelineLayout(struct xComputePipeline_t* pxComputePipeline);
VkPipeline ComputePipeline_GetPipeline(struct xComputePipeline_t* pxComputePipeline);

#endif
