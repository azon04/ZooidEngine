/***
* Default 3D Text Vertex Shader for OpenGL
* by: Ahmad Fauzan Umar
***/

#version 430 core
layout (location = 0) in vec4 PosTexCoord; // contain vec2 position and vec2 texture coordinate

out vec2 vsTexCoord;

uniform mat4 modelMat;
layout (std140) uniform shader_data
{
	mat4 viewMat;
	mat4 projectionMat;
};

void main()
{
	gl_Position = (projectionMat * viewMat * modelMat) * vec4(PosTexCoord.xy, 0.0, 1.0);
	vsTexCoord = PosTexCoord.zw;
}