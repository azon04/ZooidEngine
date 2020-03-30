/***
* Default 3D Fragment Shader for OPENGL rendering
* With Lights (lit)
* by: Ahmad Fauzan
***/

#version 430

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;
	sampler2D normalMap;
	sampler2D maskMap;

	// PBR Additional maps
	sampler2D roughnessMap;
	sampler2D metalicMap;
	sampler2D aoMap;

	float diffuseMapBound;
	float specularMapBound;
	float normalMapBound;
	bool maskMapBound;
	float roughnessMapBound;
	float metalicMapBound;
	float aoMapBound;

	float shininess;
	float metalic;
	float roughness;
	float reflectivity;

	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

#define MAX_NUM_LIGHTS 8
#define MAX_SHADOW_MAP 16

struct CascadeShadowData
{
	float cascadedDistance;
	int shadowMapIndex;
	vec2 padding;
};

struct ShadowData
{
	mat4 viewProj;
};

struct Light {

	int type; // 0 = directional Light, 1 = Point Light, 2 = Spot Light

	// For FlashLight
	float cutOff;
	float outerCutOff;

	// Use for attenuation
	float att_constant;
	float att_linear;
	float att_quadratic;

	vec3 position;
	vec3 direction; // For directional light and Flashlight
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	ivec4 shadowMapIndices;
	ivec4 shadowMapIndicesExt;
	ivec4 cascadeShadowIndices;
};

layout (std140) uniform light_data
{
	vec3 viewPos;
	int numLight;

	vec2 padding;
	int numCascade;
	
	Light lights[MAX_NUM_LIGHTS];
	CascadeShadowData cascadeShadowData[MAX_SHADOW_MAP];
	ShadowData shadowData[MAX_SHADOW_MAP];
};

uniform sampler2D shadowMaps[MAX_SHADOW_MAP];
 
out vec4 fColor;

in vec2 vsTexCoord;
in vec3 vsNormal;
in vec3 vsFragPos;
in vec3 vsTangent;
in vec3 vsBitangent;
in float vsCamDepth;

uniform Material material;

const float shadowBias = 0.00005;
const float PI = 3.14159265359;

float CalcShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int shadowMapIndex);
float CalcShadowPCF(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int shadowMapIndex);

vec3 calculateNormal()
{
	vec3 N = normalize(vsNormal);
	if(material.normalMapBound == 1.0)
	{
		vec3 normal = texture(material.normalMap, vsTexCoord).rgb * 2.0 - 1.0;
		vec3 T = normalize(vsTangent - dot(vsTangent, N) * N); // reorthogonalize T
		vec3 B = cross(N, T);
		mat3 TBN = mat3(T, B, N);
		return normalize(TBN * normal);
	}
	return N;
}

vec3 calculateAlbedo()
{
    return  mix(material.Kd, vec3(texture(material.diffuseMap, vsTexCoord)), material.diffuseMapBound);
}

float calculateMetalic()
{
	return mix(material.metalic, texture(material.metalicMap, vsTexCoord).r, material.metalicMapBound);
}

float calculateRoughness()
{
	return mix(material.roughness, texture(material.roughnessMap, vsTexCoord).r, material.roughnessMapBound);
}

vec3 calculateAmbient()
{
	return material.Ka * mix(1.0, texture(material.aoMap, vsTexCoord).r, material.aoMapBound);
}

vec3 calculateSpec()
{
    return material.Ks * mix(vec3(1.0, 1.0, 1.0), vec3(texture(material.specularMap, vsTexCoord)), material.specularMapBound);
}

float getMask()
{
	if(material.maskMapBound)
	{
		return texture(material.maskMap, vsTexCoord).r;
	}
	return 1.0;
}

vec3 fresnelShlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow((1.0 - cosTheta), 5);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num/denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1);
	float k = (r*r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1-k) + k;

	return num/denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N,V), 0.0);
	float NdotL = max(dot(N,L), 0.0);
	float ggx1 = GeometrySchlickGGX(NdotV, roughness);
	float ggx2 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 + ggx2;
}

float CalcPointAttenuation(Light light, float distance);

float calculateDirShadow(Light light, vec3 fragPos, vec3 L, vec3 normal)
{
	float depth = vsCamDepth;
	float shadow = 0.0;
	int i = 0;
	while(light.shadowMapIndices[i++] != -1)
	{
		int shadowIndex = light.shadowMapIndices[i];
		shadow = max( shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(fragPos, 1.0), normal, L, light.shadowMapIndices[i]));
	}

	const int cascadeCount = 4;
	if(light.cascadeShadowIndices[0] != -1)
	{
		int index = light.cascadeShadowIndices[0];
		ivec4 comparison = ivec4(depth <= cascadeShadowData[index].cascadedDistance, depth <= cascadeShadowData[index+1].cascadedDistance,
								depth <= cascadeShadowData[index+2].cascadedDistance, depth <= cascadeShadowData[index+3].cascadedDistance);
		int cascadeIndex = index + min(cascadeCount - (comparison.x + comparison.y + comparison.z + comparison.w), cascadeCount-1);
		int shadowIndex = cascadeShadowData[cascadeIndex].shadowMapIndex;
		shadow = max( shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(fragPos, 1.0), normal, L, shadowIndex));
	}

	return shadow;
}

float calculatePointShadow(Light light, vec3 fragPos, vec3 L, vec3 normal)
{
	float shadow = 0.0;
	for(int i=0; i < 4; i++)
	{
		if(light.shadowMapIndices[i] != -1)
		{
			int shadowIndex = light.shadowMapIndices[i];
			shadow = max(shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(fragPos, 1.0), normal, L, shadowIndex));
		}

		if(light.shadowMapIndicesExt[i] != -1)
		{
			int shadowIndex = light.shadowMapIndicesExt[i];
			shadow = max(shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(fragPos, 1.0), normal, L, shadowIndex));
		}
	}

	return shadow;
}

float calculateSpotShadow(Light light, vec3 fragPos, vec3 L, vec3 normal)
{
	float shadow = 0.0;
	for(int i=0; i < 4; i++)
	{
		if(light.shadowMapIndices[i] != -1)
		{
			int shadowIndex = light.shadowMapIndices[i];
			shadow = max(shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(fragPos, 1.0), normal, L, shadowIndex));
		}
	}
	return shadow;
}

void main()
{
	if( getMask() < 0.1 )
	{
		discard;
	}

	// properties
	vec3 normal = calculateNormal();
	vec3 fragPos = vsFragPos;
   	vec3 albedo = calculateAlbedo();
    vec3 ambient = calculateAmbient() * albedo;
	float metalic = calculateMetalic();
	float roughness = calculateRoughness();
	float f0 = material.reflectivity;
	float specularIntensity = calculateSpec().r;

	vec3 V = normalize(viewPos - fragPos);
	vec3 N = normal;

	vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metalic);

	vec3 result = vec3(0.0, 0.0, 0.0);

	for(int i=0; i < numLight; i++)
    {
		Light light = lights[i];

		vec3 L = normalize(light.position - fragPos);
		if( light.type == 0 )
		{
			L = normalize(-light.direction);
		}

		vec3 H = normalize(V + L);

        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.att_constant + light.att_linear * distance + light.att_quadratic * distance * distance);
        float intensity = 1.0;
		if( light.type == 2 )
		{
			float theta = dot(L, normalize(-light.direction));
			float epsilon = light.cutOff - light.outerCutOff;
			intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
		}
		vec3 radiance = light.diffuse * attenuation * intensity;

		if( light.type == 0 )
		{
			radiance = light.diffuse;
		}

        vec3 F = fresnelShlick(max(dot(H, V), 0.0), F0);

        float NDF = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);

        vec3 numerator = NDF * G * F;
        float denominator = 4 * max(dot(N,V), 0.0) * max(dot(N,L), 0.0);
        vec3 specular = numerator / max(denominator, 0.001) * specularIntensity;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        kD *= 1.0 - metalic;

		// calculate shadow
		float shadow = 0;
		if(light.type == 0)
		{
			shadow = calculateDirShadow(light, fragPos, L, N);
		}
		else if(light.type == 1)
		{
			shadow = calculatePointShadow(light, fragPos, L, N);
		}
		else if(light.type == 2)
		{
			shadow = calculateSpotShadow(light, fragPos, L, N);
		}

        float NdotL = max(dot(N, L), 0.0);
        result += (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);
    }

	fColor = vec4(ambient + result, 1.0f);
}

float CalcShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int shadowMapIndex)
{
	// Perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// [0,1]
	projCoords = projCoords * 0.5 + 0.5;

	//Comparison current depth and expected depth in shadow map
	float closestDepth = texture(shadowMaps[shadowMapIndex], projCoords.xy).r;
	float currentDepth = projCoords.z;

	// Calculate bias to prevent Shadow Acne
	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), shadowBias);
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	// Set shadow to 0 if the pos outside the light view frustrum
	if(projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

float CalcShadowPCF(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int shadowMapIndex)
{
	// Perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// [0,1]
	projCoords = projCoords * 0.5 + 0.5;

	//Comparison current depth and expected depth in shadow map
	float currentDepth = projCoords.z;

	// Calculate bias to prevent Shadow Acne
	float bias = max(0.0001 * (1.0 - dot(normal, lightDir)), shadowBias);
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize( shadowMaps[shadowMapIndex], 0 );
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			float closestDepth = texture(shadowMaps[shadowMapIndex], projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 25.0;

	// Set shadow to 0 if the pos outside the light view frustrum
	if(projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}

float CalcPointAttenuation(Light light, float distance)
{
	// The falloff function can be replaced to be more game-friendly rendering
	return 1.0 / (light.att_constant + light.att_linear * distance + light.att_quadratic * distance * distance);
}