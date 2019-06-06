/***
* GBuffers Pixel Shader for OPENGL
* by : Ahmad Fauzan
***/

#version 430
layout (location=0) out vec4 gPosition;
layout (location=1) out vec3 gNormal;
layout (location=2) out vec3 gAlbedo;
layout (location=3) out vec4 gSpecColor;
layout (location=4) out vec3 gAmbient;

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	sampler2D normalMap;

	float diffuseMapBound;
	float specularMapBound;
	float normalMapBound;

	float shininess;

	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

in VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
    vec3 Tangent;
    vec3 Bitangent;
	float depth;
} fs_in;

uniform Material material;

vec3 calculateNormal()
{
    vec3 normal = mix(normalize(fs_in.Normal), normalize(texture(material.normalMap, fs_in.TexCoord).rgb * 2.0 - 1.0), material.normalMapBound );
	if(material.normalMapBound == 1.0)
	{
		mat3 TBN = mat3(normalize(fs_in.Tangent), normalize(fs_in.Bitangent), normalize(normal));
		normal = normalize(TBN * normal);
	}
	return normal;
}

vec3 calculateAlbedo()
{
    return material.Kd * mix(vec3(1.0, 1.0, 1.0), vec3(texture(material.diffuseMap, fs_in.TexCoord)), material.diffuseMapBound);
}

vec3 calculateSpec()
{
    return material.Ks * mix(vec3(1.0, 1.0, 1.0), vec3(texture(material.specularMap, fs_in.TexCoord)), material.specularMapBound);
}

void main()
{
    gPosition = vec4(fs_in.FragPos, fs_in.depth);
    gNormal = calculateNormal();
    gAlbedo = calculateAlbedo();
    gSpecColor = vec4(calculateSpec(), material.shininess);
	gAmbient = material.Ka;
}