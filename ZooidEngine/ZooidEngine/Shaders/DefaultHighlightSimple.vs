/***
* Default highlight vertex shader for OPENGL
* by Ahmad Fauzan
***/

#version 330
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Noarmal; // This isn't used
layout (location = 2) in vec2 TexCoord; // This isn't used

uniform mat4 modelMat;
layout (std140) uniform shader_data
{
	mat4 viewMat;
	mat4 projectionMat;
};

void main()
{
	gl_Position = (projectionMat * viewMat * modelMat ) * vec4(Pos, 1.0);
}