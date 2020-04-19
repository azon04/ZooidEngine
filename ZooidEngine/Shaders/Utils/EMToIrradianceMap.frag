/*
Calculate the irradiance map from Environment map cubemap using convolution
*/
#version 430
out vec4 FragColor;

in vec3 FragPos;

uniform samplerCube environmentMap;

const float PI = 3.14159265359;

void main()
{
    // sample direction = the hemisphere orientation
    vec3 normal = normalize(FragPos);

    vec3 irradiance = vec3(0.0);
    vec3 up =  vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, normal);
    up = cross(normal, right);

    float sampleDelta = 0.025;
    float nrSample = 0.0;
    for(float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // Spherical to cartesan in tangent space
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // tangent space to world
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

            irradiance += texture(environmentMap, sampleVec).rgb * cos(theta) * sin(theta);
            nrSample++;
        }
    }

    irradiance = PI * irradiance * (1.0 / nrSample);

    FragColor = vec4(irradiance, 1.0);
}