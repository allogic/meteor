#version 450 core

layout(location = 0) in Vertex {
	vec4 position;
	vec4 color;
} inputVertex;

layout(location = 0) out vec4 outputColor;

void main() {
	outputColor = inputVertex.color;
}