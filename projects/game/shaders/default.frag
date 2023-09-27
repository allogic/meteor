#version 450 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputUv;
layout(location = 2) in vec4 inputColor;

layout(binding = 1) uniform sampler2D textureSampler;

layout(location = 0) out vec4 outputColor;

void main() {
	outputColor = texture(textureSampler, inputUv);
}