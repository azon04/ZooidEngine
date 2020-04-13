#version 430
out vec4 FragColor;

in vec3 FragPos;

uniform sampler2D equirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = ( vec2(0.0, 1.0) - SampleSphericalMap(normalize(FragPos)) ) * vec2(-1.0, 1.0); // GL: Need to flip the texture y
    vec3 color = texture(equirectangularMap, uv).rgb;

    FragColor = vec4(color, 1.0);
}