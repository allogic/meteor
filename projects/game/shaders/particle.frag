#version 450 core

#define TEXTURE_COUNT 1

layout(location = 0) in Vertex {
	vec4 position;
	vec2 uv;
	vec4 color;
} inputVertex;

layout(push_constant) uniform PerObjectData {
	mat4 model;
	uint textureIndex;
} perObjectData;

layout(binding = 2) uniform sampler texSampler;
layout(binding = 3) uniform texture2D Textures[TEXTURE_COUNT];

layout(location = 0) out vec4 outputColor;

void main() {
	outputColor = texture(sampler2D(Textures[perObjectData.textureIndex], texSampler), inputVertex.uv);
}