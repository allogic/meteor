#ifndef METEOR_VULKAN_GRAPHICPIPELINE_H
#define METEOR_VULKAN_GRAPHICPIPELINE_H

#include <stdint.h>

#include <vulkan/vulkan.h>

struct xInstance_t;
struct xSwapChain_t;
struct xGraphicPipeline_t;

struct xGraphicPipeline_t* GraphicPipeline_Alloc(struct xInstance_t* pxInstance, struct xSwapChain_t* pxSwapChain, VkShaderModule xVertModule, VkShaderModule xFragModule, VkPrimitiveTopology xPrimitiveTopology, VkVertexInputBindingDescription* pxVertexInputBindingDescriptions, uint32_t nVertexInputBindingDescriptionCount, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions, uint32_t nVertexInputAttributeDescriptionCount, VkDescriptorSetLayout xDescriptorSetLayout, VkPushConstantRange* pxPushConstantRanges, uint32_t nPushConstantRangeCount);
void GraphicPipeline_Free(struct xGraphicPipeline_t* pxGraphicPipeline, struct xInstance_t* pxInstance);

VkPipelineLayout GraphicPipeline_GetPipelineLayout(struct xGraphicPipeline_t* pxGraphicPipeline);
VkPipeline GraphicPipeline_GetPipeline(struct xGraphicPipeline_t* pxGraphicPipeline);

#endif
