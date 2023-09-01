#ifndef METEOR_VERTEX_H
#define METEOR_VERTEX_H

#include <math/vector.h>

#include <vulkan/vulkan.h>

typedef struct {
    xVec3_t Position;
    xVec2_t Uv;
    xVec4_t Color;
} xDefaultVertex_t;

typedef struct {
    xVec3_t Position;
    xVec4_t Color;
} xDebugVertex_t;

typedef struct {
    xVec3_t Position;
    xVec2_t Uv;
    xVec4_t Color;
} xInterfaceVertex_t;

void Vertex_DefaultDescription(VkVertexInputBindingDescription* pxVertexInputBindingDescription, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions);
void Vertex_DebugDescription(VkVertexInputBindingDescription* pxVertexInputBindingDescription, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions);
void Vertex_InterfaceDescription(VkVertexInputBindingDescription* pxVertexInputBindingDescription, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions);

#endif
