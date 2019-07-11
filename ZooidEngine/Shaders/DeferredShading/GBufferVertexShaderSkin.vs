/***
* Skinned GBuffers Vertex Shader for OPENGL
* by : Ahmad Fauzan
***/

#version 430
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TexCoord;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 BoneWeights;
layout (location = 5) in vec3 Tangent;

out VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
    vec3 Tangent;
    vec3 Bitangent;
	float depth;
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

const int MAX_BONES = 150;

layout (std140) uniform bone_mats_block
{ 
	mat4 boneMats[MAX_BONES];
};

void main()
{
	mat4 boneTransform = boneMats[BoneIDs[0]] * BoneWeights[0];
	boneTransform += boneMats[BoneIDs[1]] * BoneWeights[1];
	boneTransform += boneMats[BoneIDs[2]] * BoneWeights[2];
	boneTransform += boneMats[BoneIDs[3]] * BoneWeights[3];

	vec4 targetPos = (projectionMat * viewMat * modelMat) * boneTransform * vec4(Pos, 1.0f);
	mat3 inverseTransposeModel = mat3(transpose(inverse(viewMat * modelMat * boneTransform)));
	vs_out.Normal = inverseTransposeModel * Normal;
	vs_out.Tangent = inverseTransposeModel * Tangent;
	vs_out.Bitangent = cross(vs_out.Normal, vs_out.Tangent);
	vs_out.FragPos = vec3( viewMat * modelMat * boneTransform * vec4( Pos, 1.0f ) );
	vs_out.TexCoord = TexCoord;
	vs_out.depth = vs_out.FragPos.z * -1.0f;
	gl_Position = targetPos;
}