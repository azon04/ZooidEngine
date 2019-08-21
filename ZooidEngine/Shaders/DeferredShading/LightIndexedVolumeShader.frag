/***
* Light Indexed Volume Fragment Shader for OPENGL
* by: Ahmad Fauzan
***/

#version 430
out vec4 fColor;

uniform vec4 color;

void main()
{
    fColor = color;
}