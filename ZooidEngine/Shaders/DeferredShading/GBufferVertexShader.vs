/***
* GBuffers Vertex Shader for OPENGL
* by : Ahmad Fauzan
***/

#version 430
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in vec3 Tangent;
layout (location = 4) in vec3 Bitangent;

out VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
    vec3 Tangent;
} vs_out;

layout (std140) uniform frame_data
{
	mat4 viewMat;
	mat4 projectionMat;
	mat4 invViewMat;
};

layout (std140) uniform draw_data
{
	uniform mat4 modelMat;
};

void main()
{
	vec4 targetPos = (projectionMat * viewMat * modelMat) * vec4(Pos, 1.0f);
	mat3 inverseTransposeModel = mat3(transpose(inverse(viewMat * modelMat)));
	vs_out.Normal = inverseTransposeModel * Normal;
	vs_out.Tangent = inverseTransposeModel * Tangent;
	vs_out.FragPos = vec3( viewMat * modelMat * vec4( Pos, 1.0f ) );
	vs_out.TexCoord = TexCoord;
	gl_Position = targetPos;
}