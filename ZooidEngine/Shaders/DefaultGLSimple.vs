/***
* Default 3D Vertex Shader for OPENGL
* by : Ahmad Fauzan
***/

#version 430
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec2 TexCoord;

out vec4 vsColor;
out vec2 vsTexCoord;

// Per frame data
layout (std140) uniform frame_data
{
	mat4 viewMat;
	mat4 projectionMat;
};

// Per draw data
layout (std140) uniform draw_data
{
	mat4 modelMat;
};

void main()
{
	gl_Position = (projectionMat * viewMat * modelMat) * vec4(Pos, 1.0);
	vsColor = vec4(Color, 1.0);
	vsTexCoord = TexCoord;
}