/**
* SSAO Fragment Shader for OPENGL
* by: Ahmad Fauzan
**/

#version 430
out float fColor;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D noiseTexture;

// Configurable variables
const int kernelSize = 32;
const float radius = 0.5;
const float bias = 0.025;
const float power = 5.0;

layout (std140) uniform frame_data
{
	mat4 viewMat;
	mat4 projectionMat;
};

layout(std140) uniform sample_data
{
    vec4 samples[kernelSize];
};

in VS_OUT
{
    vec2 TexCoord;
} fs_in;

void main()
{
    vec2 noiseScale = vec2(textureSize(gPosition, 0)) / vec2(textureSize(noiseTexture, 0));
    vec3 fragPos = texture(gPosition, fs_in.TexCoord).xyz;
    vec3 normal = normalize(texture(gNormal, fs_in.TexCoord).xyz);
    vec3 randomVec = texture(noiseTexture, fs_in.TexCoord * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0f;
    for(int i=0; i < kernelSize; i++)
    {
        // get sample position
        vec3 sample1 = TBN * samples[i].xyz;
        sample1 = fragPos + sample1 * radius;

        vec4 offset = vec4(sample1, 1.0);
        offset = projectionMat * offset;        // From view to clip space
        offset.xyz /= offset.w;                 // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5;    // transorm to [0..1.0]

        float sampleDepth = texture(gPosition, offset.xy).z;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += ((sampleDepth >= sample1.z + bias) ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion = 1.0 - (occlusion / kernelSize);
    fColor = pow(occlusion, power);
}