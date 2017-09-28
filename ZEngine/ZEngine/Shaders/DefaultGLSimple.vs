/***
* Default 3D Vertex Shader for OPENGL
* by : Ahmad Fauzan
***/

#version 330
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec2 TexCoord;

out vec4 vsColor;
out vec2 vsTexCoord;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projectionMat;

void main()
{
	gl_Position = /*projectionMat * viewMat */ modelMat * vec4(Pos, 1.0f);
	vsColor = vec4(Color, 1.0f);
	vsTexCoord = TexCoord;
}