/**
* Default 3D Depth Vertex Shader for OPENGL
* by: Ahmad Fauzan
**/

#version 430
layout (location = 0) in vec3 Pos;

uniform mat4 modelMat;
layout (std140) uniform shader_data
{
	mat4 viewMat;
	mat4 projectionMat;
};

void main()
{
	gl_Position = (projectionMat * viewMat * modelMat) * vec4(Pos, 1.0);
}