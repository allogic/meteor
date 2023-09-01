#version 450 core

layout(location = 0) in Vertex {
	vec4 Position;
	vec4 Color;
} InputVertex;

layout(location = 0) out vec4 OutputColor;

void main() {
	OutputColor = InputVertex.Color;
}