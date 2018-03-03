/***
* Default 3D Fragment Shader for OPENGL rendering
* With Lights (lit)
* by: Ahmad Fauzan
***/

#version 330

struct Material
{
	sampler2D diffuseMap;
	sampler2D specularMap;

	float shininess;

	vec3 Ka;
	vec3 Kd;
	vec3 Ks;
};

#define MAX_NUM_LIGHTS 8

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
};

layout (std140) uniform light_data
{
	vec3 viewPos;
	int numLight;

	Light lights[MAX_NUM_LIGHTS];

};

out vec4 fColor;

in vec2 vsTexCoord;
in vec3 vsNormal;
in vec3 vsFragPos;

uniform Material material;

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// properties
	vec3 norm = normalize(vsNormal);
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

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	// diffuse shading
	float diff = max( dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow( max( dot( viewDir, reflectDir ), 0.0), material.shininess );
	// combine results
	vec3 ambient = light.ambient * material.Ka * vec3(texture(material.diffuseMap, vsTexCoord));
	vec3 diffuse = light.diffuse * diff * material.Kd * vec3(texture(material.diffuseMap, vsTexCoord));
	vec3 specular = light.specular * spec * material.Ks * vec3(texture(material.specularMap, vsTexCoord));
	
	return (ambient + diffuse + specular);
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
	vec3 ambient = light.ambient * material.Ka * vec3(texture(material.diffuseMap, vsTexCoord));
	vec3 diffuse = light.diffuse * diff * material.Kd * vec3(texture(material.diffuseMap, vsTexCoord));
	vec3 specular = light.specular * spec * material.Ks * vec3(texture(material.specularMap, vsTexCoord));
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
	float spec = pow( max( dot( viewDir, reflectDir ), 0.0), material.shininess );
	// attenuation
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.att_constant + light.att_linear * distance + light.att_quadratic * distance * distance);
	// intensity
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	// combine results
	vec3 ambient = light.ambient * material.Ka * vec3(texture(material.diffuseMap, vsTexCoord));
	vec3 diffuse = light.diffuse * diff * material.Kd * vec3(texture(material.diffuseMap, vsTexCoord));
	vec3 specular = light.specular * spec * material.Ks * vec3(texture(material.specularMap, vsTexCoord));
	ambient *= attenuation;
	diffuse *= attenuation * intensity;
	specular *= attenuation * intensity;

	return (ambient + diffuse + specular);

}