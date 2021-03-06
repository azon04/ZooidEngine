/***
* Default 3D depth vertex shader for skinned vertices for OPENGL
* by: Ahmad Fauzan
**/

#version 430
layout (location = 0) in vec3 Pos;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 BoneWeights;

layout (std140) uniform frame_data
{
	mat4 viewMat;
	mat4 projectionMat;
};

const int MAX_BONES = 150;

layout (std140) uniform draw_data
{
	mat4 modelMat;
};

layout (std140) uniform bone_mats_block
{ 
	uniform mat4 boneMats[MAX_BONES];
};

void main()
{
	mat4 boneTransform = boneMats[BoneIDs[0]] * BoneWeights[0];
	boneTransform += boneMats[BoneIDs[1]] * BoneWeights[1];
	boneTransform += boneMats[BoneIDs[2]] * BoneWeights[2];
	boneTransform += boneMats[BoneIDs[3]] * BoneWeights[3];

	gl_Position = (projectionMat * viewMat * modelMat) * boneTransform * vec4(Pos, 1.0f);
}