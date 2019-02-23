#version 430 core

// #OPENGL specific

in vec4 vertexColor; // The input from vertex shader (same name and same type)

out vec4 color;

void main()
{
	color = vertexColor;
}