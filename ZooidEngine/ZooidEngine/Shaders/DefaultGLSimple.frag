/***
* Default 3D Fragment Shader for OPENGL rendering
* With Single diffuse texture
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

#define MAX_NUM_LIGHTS 8

struct Light {
	int type; // 0 = directional Light, 1 = Point Light, 2 = Spot Light

	// For FlashLight
	float cutOff;
	float outerCutOff;

	vec3 position;
	vec3 direction; // For directional light and Flashlight
	
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// Use for attenuation
	float constant;
	float linear;
	float quadratic;
};

layout (std140) uniform light_data
{
	int numLight;
	vec3 viewPos;

	Light lights[MAX_NUM_LIGHTS];
};

uniform Material material;

void main()
{
	fColor = texture(material.diffuseMap, vsTexCoord) * vsColor;
}