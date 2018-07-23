/***
* Default TextShader Vertex Shader for OpenGL
* by: Ahmad Fauzan Umar
***/

#version 330 core
layout (location = 0) in vec4 PosTexCoord; // contain vec2 position and vec2 texture coordinate

out vec2 vsTexCoord;

uniform mat4 model;
uniform float screenHeight;
uniform float screenWidth;

void main()
{
	vec3 pos3 = vec3(model* vec4(PosTexCoord.xy,1.0,1.0));
	vec2 pos = pos3.xy;
	pos.x = 2.0 * (pos.x / (screenWidth)) - 1.0;
	pos.y = 2.0 * (pos.y / (screenHeight)) - 1.0;

	gl_Position = vec4( pos, 0.0, 1.0 );
	vsTexCoord = PosTexCoord.zw;
}
