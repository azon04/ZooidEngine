/***
* Default Text MSDF Fragment Shader for OpenGL
* by: Ahmad Fauzan Umar
***/

#version 430 core

in vec2 vsTexCoord;

out vec4 fColor;

uniform sampler2D fontTexture;
uniform vec3 color;

const float threshold = 0.5;
const float offset = 0.1;

float median(float r, float g, float b)
{
	return max(min(r,g), min(max(r,g),b));
}

void main()
{
	vec3 sampled = texture(fontTexture, vsTexCoord).rgb;
	float dist = median(sampled.r, sampled.g, sampled.b);
	float alpha = smoothstep(threshold - offset, threshold + offset, dist);
	fColor = vec4(color, alpha);	
}