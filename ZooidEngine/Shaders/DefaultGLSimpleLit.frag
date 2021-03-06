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

	float diffuseMapBound;
	float specularMapBound;
	float normalMapBound;

	float shininess;

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

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
float CalcShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int shadowMapIndex);
float CalcShadowPCF(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int shadowMapIndex);
vec3 CalculateNormal();
float CalcPointAttenuation(Light light, float distance);

void main()
{
	// properties
	vec3 norm = CalculateNormal();
	vec3 viewDir = normalize(viewPos - vsFragPos);

	vec3 result = vec3(0.0, 0.0, 0.0);

	for( int i=0; i <  numLight; i++ )
	{
		if ( lights[i].type == 0 ) // Directional Light
		{
			result += CalcDirLight(lights[i], norm, viewDir);		
		}

		if ( lights[i].type == 1 ) // Point Light
		{
			result += CalcPointLight(lights[i], norm, vsFragPos, viewDir);
		}

		if( lights[i].type == 2 ) // Spot Light
		{
			result += CalcSpotLight(lights[i], norm, vsFragPos, viewDir);
		}
	}

	fColor = vec4(result, 1.0f);
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

vec3 CalculateDiffuseColor()
{
	return mix(vec3(1.0, 1.0, 1.0), vec3(texture(material.diffuseMap, vsTexCoord)), material.diffuseMapBound);
}

vec3 CalculateSpecularColor()
{
	return mix(vec3(1.0, 1.0, 1.0), vec3(texture(material.specularMap, vsTexCoord)), material.specularMapBound);
}

vec3 CalculateNormal()
{
	vec3 normal = mix(normalize(vsNormal), normalize(texture(material.normalMap, vsTexCoord).rgb * 2.0 - 1.0), material.normalMapBound );
	if(material.normalMapBound == 1.0)
	{
		mat3 TBN = mat3(normalize(vsTangent), normalize(vsBitangent), normalize(normal));
		normal = normalize(TBN * normal);
	}
	return normal;
}

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	
	// diffuse shading
	float diff = max( dot(normal, lightDir), 0.0);
	
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = material.shininess > 0.0 ? pow( max( dot( viewDir, reflectDir ), 0.0), material.shininess ) : 0;
	
	// combine results
	vec3 ambient = light.ambient * material.Ka * CalculateDiffuseColor();
	vec3 diffuse = light.diffuse * diff * material.Kd * CalculateDiffuseColor();
	vec3 specular = light.specular * spec * material.Ks * CalculateSpecularColor();
	
	// Calculate shadow
	float shadow = 0.0;
	int i = 0;
	while(light.shadowMapIndices[i++] != -1)
	{
		int shadowIndex = light.shadowMapIndices[i];
		shadow = max( shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(vsFragPos, 1.0), normal, lightDir, light.shadowMapIndices[i]));
	}

	const int cascadeCount = 4;
	if(light.cascadeShadowIndices[0] != -1)
	{
		int index = light.cascadeShadowIndices[0];
		ivec4 comparison = ivec4(vsCamDepth <= cascadeShadowData[index].cascadedDistance, vsCamDepth <= cascadeShadowData[index+1].cascadedDistance,
								vsCamDepth <= cascadeShadowData[index+2].cascadedDistance, vsCamDepth <= cascadeShadowData[index+3].cascadedDistance);
		int cascadeIndex = index + min(cascadeCount - (comparison.x + comparison.y + comparison.z + comparison.w), cascadeCount-1);
		int shadowIndex = cascadeShadowData[cascadeIndex].shadowMapIndex;
		shadow = max( shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(vsFragPos, 1.0), normal, lightDir, shadowIndex));
	}

	return ambient + (1.0 - shadow) * (diffuse + specular);
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max( dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = material.shininess > 0.0 ? pow( max( dot( viewDir, reflectDir ), 0.0), material.shininess ) : 0;
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = CalcPointAttenuation(light, distance);
	// combine results
	vec3 ambient = light.ambient * material.Ka * CalculateDiffuseColor();
	vec3 diffuse = light.diffuse * diff * material.Kd * CalculateDiffuseColor();
	vec3 specular = light.specular * spec * material.Ks * CalculateSpecularColor();
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
			shadow = max(shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(vsFragPos, 1.0), normal, lightDir, shadowIndex));
		}

		if(light.shadowMapIndicesExt[i] != -1)
		{
			int shadowIndex = light.shadowMapIndicesExt[i];
			shadow = max(shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(vsFragPos, 1.0), normal, lightDir, shadowIndex));
		}
	}

	return ambient + (1 - shadow) * (diffuse + specular);
}

float CalcPointAttenuation(Light light, float distance)
{
	// The falloff function can be replaced to be more game-friendly rendering
	return 1.0 / (light.att_constant + light.att_linear * distance + light.att_quadratic * distance * distance);
}

vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse shading
	float diff = max( dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = material.shininess > 0.0 ? pow( max( dot( viewDir, reflectDir ), 0.0), material.shininess ) : 0;
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.att_constant + light.att_linear * distance + light.att_quadratic * distance * distance);
	// intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// combine results
	vec3 ambient = light.ambient * material.Ka * CalculateDiffuseColor();
	vec3 diffuse = light.diffuse * diff * material.Kd * CalculateDiffuseColor();
	vec3 specular = light.specular * spec * material.Ks * CalculateSpecularColor();
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
			shadow = max(shadow, CalcShadowPCF(shadowData[shadowIndex].viewProj * vec4(vsFragPos, 1.0), normal, lightDir, shadowIndex));
		}
	}

	return ambient + (1.0 - shadow) * (diffuse + specular);

}