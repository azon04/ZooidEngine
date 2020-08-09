/***
* Default highlight vertex shader for OPENGL
* by Ahmad Fauzan
***/

#version 430
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
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
	vec3 shellPos = Pos;
	vec3 tPos = vec3(modelMat * vec4(shellPos, 1.0));
	mat3 inverseTransposeModel = mat3(transpose(inverse(modelMat)));
	vec3 tNormal = normalize(inverseTransposeModel * Normal);
	gl_Position = (projectionMat * viewMat) * vec4(tPos + 0.005 * tNormal, 1.0);
}