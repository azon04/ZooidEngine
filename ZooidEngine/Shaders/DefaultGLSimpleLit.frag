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

	mat4 viewProj;

	ivec4 shadowMapIndices;
};

layout (std140) uniform light_data
{
	vec3 viewPos;
	int numLight;

	Light lights[MAX_NUM_LIGHTS];
};

uniform sampler2D shadowMaps[MAX_SHADOW_MAP];
 
out vec4 fColor;

in vec2 vsTexCoord;
in vec3 vsNormal;
in vec3 vsFragPos;
in vec3 vsTangent;
in vec3 vsBitangent;

uniform Material material;

const float shadowBias = 0.002;

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
float CalcShadow(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int shadowMapIndex);
float CalcShadowPCF(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir, int shadowMapIndex);
vec3 CalculateNormal();

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
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize( shadowMaps[shadowMapIndex], 0 );
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float closestDepth = texture(shadowMaps[shadowMapIndex], projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth > closestDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

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
	for(int i=0; i < 4; i++)
	{
		if(light.shadowMapIndices[i] != -1)
		{
			shadow = max(shadow, CalcShadowPCF(light.viewProj * vec4(vsFragPos, 1.0), normal, lightDir, light.shadowMapIndices[i]));
		}
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
	float spec = pow( max( dot( viewDir, reflectDir ), 0.0), material.shininess );
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.att_constant + light.att_linear * distance + light.att_quadratic * distance * distance);
	// combine results
	vec3 ambient = light.ambient * material.Ka * CalculateDiffuseColor();
	vec3 diffuse = light.diffuse * diff * material.Kd * CalculateDiffuseColor();
	vec3 specular = light.specular * spec * material.Ks * CalculateSpecularColor();
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return (ambient + diffuse + specular);
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
			shadow = max(shadow, CalcShadowPCF(light.viewProj * vec4(vsFragPos, 1.0), normal, lightDir, light.shadowMapIndices[i]));
		}
	}

	return ambient + (1.0 - shadow) * (diffuse + specular);

}