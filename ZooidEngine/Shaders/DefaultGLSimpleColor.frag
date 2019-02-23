/***
* Default 3D Fragment Shader for OPENGL rendering
* With Single diffuse texture
* by: Ahmad Fauzan
***/

#version 430

out vec4 fColor;

in vec4 vsColor;
in vec2 vsTexCoord;

void main()
{
	fColor = vsColor;
}