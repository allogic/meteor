#version 450 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec4 inputColor;

layout(set = 0, binding = 0) uniform ViewProjection {
	mat4 view;
	mat4 projection;
} vp;

layout(location = 0) out Vertex {
	vec4 position;
	vec4 color;
} outputVertex;

void main() {
	vec4 position = vp.projection * vp.view * vec4(inputPosition, 1.0);
	vec4 color = inputColor;

	outputVertex.position = position;
	outputVertex.color = color;

	gl_Position = position;
}