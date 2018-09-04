/***
* Default 3D Text Fragment Shader for OpenGL
* by: Ahmad Fauzan Umar
***/

#version 330 core

out vec4 fColor;
in vec2 vsTexCoord;

uniform sampler2D fontTexture;
uniform vec3 color;

void main()
{
	vec4 sampled = vec4(vec3(1.0), texture(fontTexture, vsTexCoord).r);
	fColor = vec4(color, 1.0f) * sampled;
}