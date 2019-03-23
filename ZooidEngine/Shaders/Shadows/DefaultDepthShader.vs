/**
* Default 3D Depth Vertex Shader for OPENGL
* by: Ahmad Fauzan
**/

#version 430
layout (location = 0) in vec3 Pos;

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
	gl_Position = (projectionMat * viewMat * modelMat) * vec4(Pos, 1.0);
}