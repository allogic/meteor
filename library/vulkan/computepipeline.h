#ifndef METEOR_COMPUTEPIPELINE_H
#define METEOR_COMPUTEPIPELINE_H

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xComputePipeline_t;

struct xComputePipeline_t* ComputePipeline_Alloc(struct xInstance_t* pxInstance, VkShaderModule xCompModule, VkDescriptorSetLayout xDescriptorSetLayout);
void ComputePipeline_Free(struct xComputePipeline_t* pxComputePipeline, struct xInstance_t* pxInstance);

VkPipelineLayout ComputePipeline_GetPipelineLayout(struct xComputePipeline_t* pxComputePipeline);
VkPipeline ComputePipeline_GetPipeline(struct xComputePipeline_t* pxComputePipeline);

#endif
