#version 450 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputUv;
layout(location = 2) in vec4 inputColor;

layout(binding = 0) uniform UniformModelViewProjection {
	mat4 model;
	mat4 view;
	mat4 projection;
} mvp;

layout(location = 0) out Vertex {
	vec4 position;
	vec2 uv;
	vec4 color;
} outputVertex;

void main() {
	vec4 position = mvp.projection * mvp.view * mvp.model * vec4(inputPosition, 1.0);
	vec4 color = inputColor;

	outputVertex.position = position;
	outputVertex.uv = inputUv;
	outputVertex.color = color;

	gl_Position = position;
}