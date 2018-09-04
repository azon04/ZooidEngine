/***
* Default 3D Text SDF Fragment Shader for OpenGL
* by: Ahmad Fauzan Umar
***/

#version 330 core

out vec4 fColor;
in vec2 vsTexCoord;

uniform sampler2D fontTexture;
uniform vec3 color;

const float threshold = 0.5;
const float offset = 0.1;

void main()
{
	float dist = texture(fontTexture, vsTexCoord).r;
	float alpha = smoothstep(threshold - offset, threshold + offset, dist);
	fColor = vec4(color, alpha);	
}