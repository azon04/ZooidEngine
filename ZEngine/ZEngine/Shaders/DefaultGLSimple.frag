/***
* Default 3D Fragment Shader for OPENGL rendering
* With Single diffuse texture
* by: Ahmad Fauzan
***/

#version 330

struct Material
{
	sampler2D diffuseMap;
}

out vec4 fColor;

in vec4 vsColor;
in vec2 vsTexCoord;

uniform Material material;

void main()
{
	fColor = vsColor;
}