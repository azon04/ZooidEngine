/***
* GBuffers Pixel Shader for OPENGL
* by : Ahmad Fauzan
***/

#version 430
layout (location=0) out vec3 gPosition; // in View-Space
layout (location=1) out vec3 gNormal; // In View-Space
layout (location=2) out vec3 gAlbedo;
layout (location=3) out vec4 gSpecColor;
layout (location=4) out vec3 gAmbient;

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

in VS_OUT
{
    vec2 TexCoord;
    vec3 Normal;
    vec3 FragPos;
    vec3 Tangent;
} fs_in;

uniform Material material;

float getMask()
{
	if(material.maskMapBound)
	{
		return texture(material.maskMap, fs_in.TexCoord).r;
	}
	return 1.0;
}

vec3 calculateNormal()
{
	vec3 N = normalize(fs_in.Normal);
    
	if(material.normalMapBound == 1.0)
	{
		vec3 normal = texture(material.normalMap, fs_in.TexCoord).rgb * 2.0 - 1.0;
		vec3 T = normalize(fs_in.Tangent - dot(fs_in.Tangent, N) * N); // reorthogonalize T
		vec3 B = cross(N, T);
		mat3 TBN = mat3(T, B, N);
		return normalize(TBN * normal);
	}
	else
	{
		return N;
	}
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
	if(getMask() < 0.1)
	{
		discard;
	}
    gPosition = fs_in.FragPos;
    gNormal = calculateNormal();
    gAlbedo = calculateAlbedo();
    gSpecColor = vec4(calculateSpec(), material.shininess);
	gAmbient = material.Ka;
}