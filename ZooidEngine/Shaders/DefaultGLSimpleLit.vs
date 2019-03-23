/***
* Default 3D Vertex Shader for OPENGL support Lit fragment shader
* by : Ahmad Fauzan
***/

#version 430
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;

out vec2 vsTexCoord;
out vec3 vsNormal;
out vec3 vsFragPos;

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
	vsNormal = mat3(transpose(inverse(modelMat))) * Normal;
	vsFragPos = vec3( modelMat * vec4( Pos, 1.0f ) );
	vsTexCoord = TexCoord;
}