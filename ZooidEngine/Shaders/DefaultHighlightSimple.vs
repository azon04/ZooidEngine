/***
* Default highlight vertex shader for OPENGL
* by Ahmad Fauzan
***/

#version 430
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Noarmal; // This isn't used
layout (location = 2) in vec2 TexCoord; // This isn't used

layout (std140) uniform frame_data
{
	mat4 viewMat;
	mat4 projectionMat;
};

layout (std140) uniform draw_data
{
	mat4 modelMat;
};

void main()
{
	gl_Position = (projectionMat * viewMat * modelMat ) * vec4(Pos, 1.0);
}