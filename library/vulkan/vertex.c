#include <vulkan/vertex.h>

void Vertex_DefaultDescription(VkVertexInputBindingDescription* pxVertexInputBindingDescription, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions) {
	pxVertexInputBindingDescription->binding = 0;
	pxVertexInputBindingDescription->stride = sizeof(xDefaultVertex_t);
	pxVertexInputBindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	pxVertexInputAttributeDescriptions[0].binding = 0;
	pxVertexInputAttributeDescriptions[0].location = 0;
	pxVertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	pxVertexInputAttributeDescriptions[0].offset = 0;

	pxVertexInputAttributeDescriptions[1].binding = 0;
	pxVertexInputAttributeDescriptions[1].location = 1;
	pxVertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	pxVertexInputAttributeDescriptions[1].offset = sizeof(float) * 3;

	pxVertexInputAttributeDescriptions[2].binding = 0;
	pxVertexInputAttributeDescriptions[2].location = 2;
	pxVertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	pxVertexInputAttributeDescriptions[2].offset = sizeof(float) * 5;
}

void Vertex_DebugDescription(VkVertexInputBindingDescription* pxVertexInputBindingDescription, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions) {
	pxVertexInputBindingDescription->binding = 0;
	pxVertexInputBindingDescription->stride = sizeof(xDebugVertex_t);
	pxVertexInputBindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	pxVertexInputAttributeDescriptions[0].binding = 0;
	pxVertexInputAttributeDescriptions[0].location = 0;
	pxVertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	pxVertexInputAttributeDescriptions[0].offset = 0;

	pxVertexInputAttributeDescriptions[1].binding = 0;
	pxVertexInputAttributeDescriptions[1].location = 1;
	pxVertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	pxVertexInputAttributeDescriptions[1].offset = sizeof(float) * 3;
}

void Vertex_InterfaceDescription(VkVertexInputBindingDescription* pxVertexInputBindingDescription, VkVertexInputAttributeDescription* pxVertexInputAttributeDescriptions) {
	pxVertexInputBindingDescription->binding = 0;
	pxVertexInputBindingDescription->stride = sizeof(xInterfaceVertex_t);
	pxVertexInputBindingDescription->inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	pxVertexInputAttributeDescriptions[0].binding = 0;
	pxVertexInputAttributeDescriptions[0].location = 0;
	pxVertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	pxVertexInputAttributeDescriptions[0].offset = 0;

	pxVertexInputAttributeDescriptions[1].binding = 0;
	pxVertexInputAttributeDescriptions[1].location = 1;
	pxVertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	pxVertexInputAttributeDescriptions[1].offset = sizeof(float) * 3;

	pxVertexInputAttributeDescriptions[2].binding = 0;
	pxVertexInputAttributeDescriptions[2].location = 2;
	pxVertexInputAttributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
	pxVertexInputAttributeDescriptions[2].offset = sizeof(float) * 5;
}
