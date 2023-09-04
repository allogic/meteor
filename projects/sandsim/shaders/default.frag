#version 450 core

layout(location = 0) in Vertex {
	vec4 Position;
	vec2 Uv;
	vec4 Color;
} inputVertex;

layout(binding = 1) uniform sampler2D uniformSampler;

layout(location = 0) out vec4 outputColor;

void main() {
	outputColor = texture(uniformSampler, inputVertex.Uv);
}