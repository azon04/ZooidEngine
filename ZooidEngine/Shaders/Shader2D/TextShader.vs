/***
* Default TextShader Vertex Shader for OpenGL
* by: Ahmad Fauzan Umar
***/

#version 330 core
layout (location = 0) in vec4 PosTexCoord; // contain vec2 position and vec2 texture coordinate

out vec2 vsTexCoord;

uniform float screenHeight;
uniform float screenWidth;

void main()
{
	vec2 pos = PosTexCoord.xy;
	pos.x = 2.0 * (pos.x / (screenWidth * 0.5)) - 1.0;
	pos.y = 1.0 - 2.0 * (pos.y / (screenHeight * 0.5));

	gl_Position = vec4( pos, 0.0, 1.0 );
	vsTexCoord = PosTexCoord.zw;
}
