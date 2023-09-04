#ifndef METEOR_VERTEX_H
#define METEOR_VERTEX_H

#include <math/vector.h>

#include <vulkan/vulkan.h>

#define VERTEX_STRUCT { \
    xVec3_t xPosition; \
    xVec2_t xUv; \
    xVec4_t xColor; \
}

#ifdef OS_WINDOWS
#pragma pack(push, 1)
	typedef struct VERTEX_STRUCT xVertex_t;
#pragma pack(pop)
#endif

#ifdef OS_LINUX
	typedef struct __attribute__((packed)) VERTEX_STRUCT xVertex_t;
#endif

void Vertex_CreateDescription(VkVertexInputBindingDescription* pxVertexInputBindingDescription, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions);

#endif
