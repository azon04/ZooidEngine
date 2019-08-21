/***
* Default 3D Vertex Shader for OPENGL support Lit vertex shader
* by : Ahmad Fauzan
***/

#version 430
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

out vec2 vsTexCoord;
out vec3 vsNormal;
out vec3 vsFragPos;
out vec3 vsTangent;
out vec3 vsBitangent;
out float vsCamDepth;

layout (std140) uniform frame_data
{
	mat4 viewMat;
	mat4 projectionMat;
};

layout (std140) uniform draw_data
{
	uniform mat4 modelMat;
};

void main()
{
	gl_Position = (projectionMat * viewMat * modelMat) * vec4(Pos, 1.0f);
	mat3 inverseTransposeModel = mat3(transpose(inverse(modelMat)));
	vsNormal = inverseTransposeModel * Normal;
	vsTangent = inverseTransposeModel * Tangent;
	vsBitangent = cross(vsNormal, vsTangent);
	vsFragPos = vec3( modelMat * vec4( Pos, 1.0f ) );
	vsTexCoord = TexCoord;
	vsCamDepth = vec3( viewMat * vec4( vsFragPos, 1.0f) ).z * -1.0f;
}