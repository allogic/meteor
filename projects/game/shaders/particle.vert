#version 450 core

struct Particle {
	vec3 position;
	vec3 velocity;
};

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputUv;
layout(location = 2) in vec4 inputColor;

layout(push_constant) uniform PerObjectData {
	mat4 model;
} perObjectData;

layout(binding = 0) uniform ViewProjection {
	mat4 view;
	mat4 projection;
} vp;

layout(std430, binding = 1) readonly buffer InputParticle {
	Particle particles[];
};

layout(location = 0) out Vertex {
	vec4 position;
	vec2 uv;
	vec4 color;
} outputVertex;

void main() {
	uint index = gl_InstanceIndex;

	vec3 instancePosition = particles[index].position;

	vec4 position = vp.projection * vp.view * perObjectData.model * vec4(inputPosition + instancePosition, 1.0);
	vec4 color = inputColor;

	outputVertex.position = position;
	outputVertex.uv = inputUv;
	outputVertex.color = color;

	gl_Position = position;
}