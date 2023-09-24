#version 450 core

layout(location = 0) in Vertex {
	vec4 position;
	vec2 uv;
	vec4 color;
} inputVertex;

layout(set = 0, binding = 2) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outputColor;

void main() {
	outputColor = texture(textureSampler, inputVertex.uv);
}