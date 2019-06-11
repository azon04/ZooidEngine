/**
* Screen Fragment Shader for OPENGL
* by: Ahmad Fauzan
**/

#version 430
out vec4 fColor;

uniform sampler2D finalTexture;

uniform bool hdrToneMap;
uniform float exposure;

in VS_OUT
{
    vec2 TexCoord;
} fs_in;

void main()
{
    vec3 color = texture(finalTexture, fs_in.TexCoord).rgb;

    if(hdrToneMap)
    {
        // Exposure tone mapping
        color = vec3(1.0) - exp(-color*exposure);
    }

    // Gamma correction
    color = pow(color, vec3(1.0 / 2.2));
    
    fColor = vec4(color, 1.0);
}