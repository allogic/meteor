#version 450 core

layout(location = 0) in Vertex {
	vec4 Position;
	vec2 Uv;
	vec4 Color;
} InputVertex;

layout(binding = 1) uniform sampler2D UniformSampler;

layout(location = 0) out vec4 OutputColor;

void main() {
	OutputColor = texture(UniformSampler, InputVertex.Uv);
}