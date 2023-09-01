#version 450 core

layout(location = 0) in Vertex {
	vec4 Position;
	vec4 Color;
} vertex;

layout(location = 0) out vec4 OutputColor;

void main() {
	OutputColor = vertex.Color;
}