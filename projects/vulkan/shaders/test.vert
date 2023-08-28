#version 450 core

layout (location = 0) in vec3 InputPosition;
layout (location = 1) in vec2 InputUv;
layout (location = 2) in vec4 InputColor;

layout (location = 0) out Vertex
{
	vec3 Position;
	vec4 Color;
} vertex;

void main() {
	vertex.Position = InputPosition;
	vertex.Color = InputColor;

	gl_Position = vec4(InputPosition, 1.0);
}