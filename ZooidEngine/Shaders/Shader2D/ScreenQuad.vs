/***
* Default Screen Quad Vertex Shader for OPENGL
* Ahmad Fauzan
***/

#version 430
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 TexCoord;

out vec2 vsTexCoord;

uniform vec2 offsetPos;
uniform vec2 dimension;

void main()
{
    gl_Position = vec4((Pos * vec3(dimension, 1.0f)) + vec3(offsetPos, 0.0f), 1.0f);
    vsTexCoord = TexCoord;
}