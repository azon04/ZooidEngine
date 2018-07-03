/***
* Default highlight fragment shader for OPENGL
* by Ahmad Fauzan
***/

#version 330

out vec4 fColor;

uniform vec3 highlightColor;

void main()
{
	fColor = vec4(highlightColor, 1.0);
}