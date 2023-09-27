#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUv;
layout(location = 2) in vec4 vertexColor;

layout(binding = 0) uniform ViewProjection {
	mat4 view;
	mat4 projection;
} vp;

layout(location = 0) out vec3 outputPosition;
layout(location = 1) out vec2 outputUv;
layout(location = 2) out vec4 outputColor;

void main() {
	vec4 position = vp.projection * vp.view * vec4(vertexPosition, 1.0);

	outputPosition = vec3(position);
	outputUv = vertexUv;
	outputColor = vertexColor;

	gl_Position = position;
}