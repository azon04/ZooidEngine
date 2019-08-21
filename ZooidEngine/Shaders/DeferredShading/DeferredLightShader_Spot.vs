/***
* Light Volume Shader for OPENGL support Lit vertex shader
* by : Ahmad Fauzan
***/

#version 430
layout (location = 0) in vec3 Pos;

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

out VS_OUT
{
    vec4 projectionSpace;
} vs_out;

void main()
{
	gl_Position = (projectionMat * viewMat * modelMat) * vec4(Pos, 1.0f);
    vs_out.projectionSpace = gl_Position;
    vs_out.projectionSpace.xy = (vs_out.projectionSpace.xy + vec2(vs_out.projectionSpace.w)) * 0.5; 
}