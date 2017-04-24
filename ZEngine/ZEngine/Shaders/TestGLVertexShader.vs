#version 330 core

// #OPENGL Specifics

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

out vec4 vertexColor;

void main() {
	gl_Position = vec4(position, 1.0);
	vertexColor = vec4(color, 1.0);
}