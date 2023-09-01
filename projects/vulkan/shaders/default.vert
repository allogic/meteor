#version 450 core

layout(location = 0) in vec3 InputPosition;
layout(location = 1) in vec2 InputUv;
layout(location = 2) in vec4 InputColor;

layout(binding = 0) uniform ModelViewProjection {
	mat4 Model;
	mat4 View;
	mat4 Projection;
} mvp;

layout(location = 0) out Vertex {
	vec4 Position;
	vec2 Uv;
	vec4 Color;
} vertex;

void main() {
	vec4 position = mvp.Projection * mvp.View * mvp.Model * vec4(InputPosition, 1.0);
	vec4 color = InputColor;

	vertex.Position = position;
	vertex.Uv = InputUv;
	vertex.Color = color;

	gl_Position = position;
}