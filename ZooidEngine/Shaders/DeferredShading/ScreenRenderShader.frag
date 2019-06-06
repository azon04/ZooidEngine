/**
* Screen Fragment Shader for OPENGL
* by: Ahmad Fauzan
**/

#version 430
out vec4 fColor;

uniform sampler2D finalTexture;

in VS_OUT
{
    vec2 TexCoord;
} fs_in;

void main()
{
    fColor = vec4(texture(finalTexture, fs_in.TexCoord).rgb, 1.0);
}