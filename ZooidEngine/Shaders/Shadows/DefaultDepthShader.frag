/***
* Default 3D Depth Fragment shader for OPENGL
* by Ahmad Fauzan
**/

#version 430

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	sampler2D normalMap;
	sampler2D maskMap;

	float diffuseMapBound;
	float specularMapBound;
	float normalMapBound;
	bool maskMapBound;

	float shininess;

	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

uniform Material material;

in VS_OUT
{
    vec2 TexCoord;
} fs_in;

float getMask()
{
	if(material.maskMapBound)
	{
		return texture(material.maskMap, fs_in.TexCoord).r;
	}
	return 1.0;
}

void main()
{
	if(getMask() < 0.1) { discard; }
	//gl_FragDepth = gl_FragCoord.z * 0.97;
}