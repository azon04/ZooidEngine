/**
* Deferred Light Fragment Shader for OPENGL
* by : Ahmad Fauzan
**/

#version 430
out vec4 fColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gSpec;
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

in VS_OUT
{
    vec2 TexCoord;
} fs_in;

const float shadowBias = 0.0005;

vec3 CalcDirLight(Light light, vec3 normal, vec3 fragPos, float depth, vec3 viewDir, vec3 albedo, vec3 specIntensity, float specPower, vec3 ambient, float occlusion);
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, vec3 specIntensity, float specPower, vec3 ambient, float occlusion);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, vec3 specIntensity, float specPower, vec3 ambient, float occlusion);
float CalcShadowPCF(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int shadowMapIndex);

void main()
{
	vec3 norm = normalize(mat3(invViewMat) * texture(gNormal, fs_in.TexCoord).xyz); // To World Space
    vec3 fragPos = (invViewMat * vec4(texture(gPosition, fs_in.TexCoord).xyz, 1.0)).xyz; // To World Space
    float depth = texture(gPosition, fs_in.TexCoord).w;
	vec3 viewDir = normalize(viewPos - fragPos);
    vec3 albedo = texture(gAlbedo, fs_in.TexCoord).rgb;
    vec3 ambient = texture(gAmbient, fs_in.TexCoord).rgb;
    vec3 specIntensity = texture(gSpec, fs_in.TexCoord).rgb;
    float specPower = texture(gSpec, fs_in.TexCoord).a;
	float occlusion = texture(gSSAO, fs_in.TexCoord).r;
    
	vec3 result = vec3(0.0, 0.0, 0.0);

	for( int i=0; i <  numLight; i++ )
	{
		if ( lights[i].type == 0 ) // Directional Light
		{
			result += CalcDirLight(lights[i], norm, fragPos, depth, viewDir, albedo, specIntensity, specPower, ambient, occlusion);		
		}
        else if ( lights[i].type == 1 ) // Point Light
		{
			result += CalcPointLight(lights[i], norm, fragPos, viewDir, albedo, specIntensity, specPower, ambient, occlusion);
		}
        else if( lights[i].type == 2 ) // Spot Light
		{
			result += CalcSpotLight(lights[i], norm, fragPos, viewDir, albedo, specIntensity, specPower, ambient, occlusion);
		}
	}

	fColor = vec4(result, 1.0f);
}

vec3 CalcDirLight(Light light, vec3 normal, vec3 fragPos, float depth, vec3 viewDir, vec3 albedo, vec3 specIntensity, float specPower, vec3 ambientCoef, float occlusion)
{
	vec3 lightDir = normalize(-light.direction);
	
	// diffuse shading
	float diff = max( dot(normal, lightDir), 0.0);
	
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = specPower > 0.0 ? pow( max( dot( viewDir, reflectDir ), 0.0), specPower ) : 0;
	
	// combine results
	vec3 ambient = light.ambient * albedo * ambientCoef;
	vec3 diffuse = light.diffuse * diff * albedo;
	vec3 specular = light.specular * spec * specIntensity;

    // Calculate shadow
	float shadow = 0.0;
	int i = 0;
	while(light.shadowMapIndices[i++] != -1)
	{
		int shadowIndex = light.shadowMapIndices[i];
		shadow = max( shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(fragPos, 1.0), normal, lightDir, light.shadowMapIndices[i]));
	}

	const int cascadeCount = 4;
	if(light.cascadeShadowIndices[0] != -1)
	{
		int index = light.cascadeShadowIndices[0];
		ivec4 comparison = ivec4(depth <= cascadeShadowData[index].cascadedDistance, depth <= cascadeShadowData[index+1].cascadedDistance,
								depth <= cascadeShadowData[index+2].cascadedDistance, depth <= cascadeShadowData[index+3].cascadedDistance);
		int cascadeIndex = index + min(cascadeCount - (comparison.x + comparison.y + comparison.z + comparison.w), cascadeCount-1);
		int shadowIndex = cascadeShadowData[cascadeIndex].shadowMapIndex;
		shadow = max( shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(fragPos, 1.0), normal, lightDir, shadowIndex));
	}

	return (ambient*occlusion) + (1.0 - shadow) * (diffuse + specular);
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, vec3 specIntensity, float specPower, vec3 ambientCoef, float occlusion)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max( dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = specPower > 0 ? pow( max( dot( viewDir, reflectDir ), 0.0), specPower ) : 0;
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.att_constant + light.att_linear * distance + light.att_quadratic * distance * distance);
	// combine results
	vec3 ambient = light.ambient * albedo * ambientCoef;
	vec3 diffuse = light.diffuse * diff * albedo;
	vec3 specular = light.specular * spec * specIntensity;
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

    // Calculate shadow
	float shadow = 0.0;
	for(int i=0; i < 4; i++)
	{
		if(light.shadowMapIndices[i] != -1)
		{
			int shadowIndex = light.shadowMapIndices[i];
			shadow = max(shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(fragPos, 1.0), normal, lightDir, shadowIndex));
		}

		if(light.shadowMapIndicesExt[i] != -1)
		{
			int shadowIndex = light.shadowMapIndicesExt[i];
			shadow = max(shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(fragPos, 1.0), normal, lightDir, shadowIndex));
		}
	}

	return (ambient * occlusion) + (1.0 - shadow) * (diffuse + specular);
}

vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir, vec3 albedo, vec3 specIntensity, float specPower, vec3 ambientCoef, float occlusion)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max( dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = specPower > 0.0 ? pow( max( dot( viewDir, reflectDir ), 0.0), specPower ) : 0;
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.att_constant + light.att_linear * distance + light.att_quadratic * distance * distance);
	// intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// combine results
	vec3 ambient = light.ambient * albedo * ambientCoef;
	vec3 diffuse = light.diffuse * diff * albedo;
	vec3 specular = light.specular * spec * specIntensity;
	ambient *= attenuation;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

    // Calculate shadow
	float shadow = 0.0;
	for(int i=0; i < 4; i++)
	{
		if(light.shadowMapIndices[i] != -1)
		{
			int shadowIndex = light.shadowMapIndices[i];
			shadow = max(shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(fragPos, 1.0), normal, lightDir, shadowIndex));
		}
	}

	return ( occlusion * ambient ) + (1.0 - shadow) * (diffuse + specular);

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