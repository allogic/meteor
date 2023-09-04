#version 450 core

layout(location = 0) in vec3 inputPosition;
layout(location = 1) in vec2 inputUv;
layout(location = 2) in vec4 inputColor;

layout(binding = 0) uniform UniformModelViewProjection {
	mat4 Model;
	mat4 View;
	mat4 Projection;
} mvp;

layout(location = 0) out Vertex {
	vec4 Position;
	vec2 Uv;
	vec4 Color;
} outputVertex;

void main() {
	vec4 position = mvp.Projection * mvp.View * mvp.Model * vec4(inputPosition, 1.0);
	vec4 color = inputColor;

	outputVertex.Position = position;
	outputVertex.Uv = inputUv;
	outputVertex.Color = color;

	gl_Position = position;
}