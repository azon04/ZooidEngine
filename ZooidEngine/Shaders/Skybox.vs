/***
* Skybox vertex shader for OPENGL
* by Ahmad Fauzan
***/

#version 440 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140) uniform shader_data
{
	mat4 viewMat;
	mat4 projectionMat;
};

void main()
{
	TexCoords = aPos;
    mat4 newView = mat4(mat3(viewMat));
    newView[3][3] = 1.0;
	vec4 pos = projectionMat * newView * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}