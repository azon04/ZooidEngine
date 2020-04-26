
#version 430
out vec4 fColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gMetalRough;
uniform sampler2D gAmbient;
uniform sampler2D gSSAO;
uniform samplerCube gIrradianceMap;

layout (std140) uniform frame_data
{
    mat4 viewMat;
    mat4 projectionMat;
    mat4 invViewMat;
};

in VS_OUT
{
    vec2 TexCoord;
} fs_in;

vec3 freshnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    vec3 normal = normalize(mat3(invViewMat) * texture(gNormal, fs_in.TexCoord).xyz); // To World Space
    vec3 fragPos = (invViewMat * vec4(texture(gPosition, fs_in.TexCoord).xyz, 1.0)).xyz; // To World Space
    vec3 albedo = texture(gAlbedo, fs_in.TexCoord).rgb;
    vec3 ambient = texture(gAmbient, fs_in.TexCoord).rgb;
	vec4 metalRoughnessF = texture(gMetalRough, fs_in.TexCoord);
	float metalic = metalRoughnessF.r;
	float roughness = metalRoughnessF.g;
	float f0 = metalRoughnessF.b;
	float occlusion = texture(gSSAO, fs_in.TexCoord).r;
    vec3 viewPos = invViewMat[3].xyz;

    vec3 N = normal;
    vec3 V = normalize(viewPos - fragPos);

    vec3 F0 = vec3(f0);
    F0 = mix(F0, albedo, metalic);

    vec3 kS = freshnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(gIrradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;
    vec3 ao = ambient * occlusion;
    vec3 color = (kD * diffuse) * ao;

    fColor = vec4(color, 1.0);
}