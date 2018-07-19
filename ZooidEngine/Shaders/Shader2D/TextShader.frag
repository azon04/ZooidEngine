/***
* Default TextShader Fragment Shader for OpenGL
* by: Ahmad Fauzan Umar
***/

#version 330 core

in vec2 vsTexCoord;

out vec4 fColor;

uniform sampler2D fontTexture;
uniform vec3 color;

void main()
{
	vec3 outColor = texture(fontTexture, vsTexCoord).r * color;
	fColor = vec4(outColor, 1.0);	
}