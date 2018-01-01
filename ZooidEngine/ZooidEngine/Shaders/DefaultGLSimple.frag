/***
* Default 3D Fragment Shader for OPENGL rendering
* With Basic Light (Unlit)
* by: Ahmad Fauzan
***/

#version 330

struct Material
{
	sampler2D diffuseMap;
};

out vec4 fColor;

in vec4 vsColor;
in vec2 vsTexCoord;

uniform Material material;

void main()
{
	fColor = texture(material.diffuseMap, vsTexCoord) * vsColor;
}