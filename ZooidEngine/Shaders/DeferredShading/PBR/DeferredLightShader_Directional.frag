/**
* Deferred Light Fragment Shader for Directional Light - PBR Variant
* by : Ahmad Fauzan
**/

#version 430
out vec4 fColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalRough;
uniform sampler2D gAmbient;
uniform sampler2D gSSAO;

#define MAX_SHADOW_MAP 16
#define MAX_NUM_LIGHTS 8

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

struct Light
{
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

layout (std140) uniform frame_data
{
	mat4 viewMat;
	mat4 projectionMat;
	mat4 invViewMat;
};

layout (std140) uniform light_sample_data
{
    vec3 viewPos;
    float padding;
    
	Light light;
	CascadeShadowData cascadeShadowData[4];
	ShadowData shadowData[4];
};

uniform sampler2D shadowMaps[MAX_SHADOW_MAP];

in VS_OUT
{
    vec2 TexCoord;
} fs_in;

const float shadowBias = 0.0005;
const float PI = 3.14159265359;

float CalcShadowPCF(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int shadowMapIndex);

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

	return ggx1 * ggx2;
}

void main()
{
	vec3 normal = normalize(mat3(invViewMat) * texture(gNormal, fs_in.TexCoord).xyz); // To World Space
    vec3 fragPos = (invViewMat * vec4(texture(gPosition, fs_in.TexCoord).xyz, 1.0)).xyz; // To World Space
    float depth = texture(gPosition, fs_in.TexCoord).z * -1.0; // Since the gPosition in View Space, let's get the z value of that for camera depth
	vec3 albedo = texture(gAlbedo, fs_in.TexCoord).rgb;
	vec4 metalRoughnessF = texture(gMetalRough, fs_in.TexCoord);
	float metalic = metalRoughnessF.r;
	float roughness = metalRoughnessF.g;
	float f0 = metalRoughnessF.b;
	float specularIntensity = metalRoughnessF.a;
    
	vec3 result = vec3(0.0);
	
	vec3 L = normalize(-light.direction);
	vec3 V = normalize(viewPos - fragPos);
	vec3 N = normal;
	vec3 H = normalize(V + L);

	float attenuation = 1.0;
	vec3 radiance = light.diffuse * attenuation;
	
	vec3 F0 = vec3(f0);
	F0 = mix(F0, albedo, metalic);
	vec3 F = fresnelShlick(max(dot(H,V), 0.0), F0);

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);

	vec3 numerator = NDF * G * F;
	float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
	vec3 specular = numerator / max(denominator, 0.001) * specularIntensity;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;

	kD *= 1.0 - metalic;

    // Calculate shadow
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
	
	float NdotL = max(dot(N,L), 0.0);
	result = (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow);

	fColor = vec4(result, 1.0f);
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
	float bias = max(0.01 * (1.0 - dot(normal, lightDir)), shadowBias);
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize( shadowMaps[shadowMapIndex], 0 );
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float closestDepth = texture(shadowMaps[shadowMapIndex], projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > closestDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	// Set shadow to 0 if the pos outside the light view frustrum
	if(projCoords.z > 1.0)
		shadow = 0.0;

	return shadow;
}