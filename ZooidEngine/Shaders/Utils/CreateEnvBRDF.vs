#version 430
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoord;

out VS_OUT
{
    vec2 TexCoord;
} vs_out;

void main()
{
    gl_Position = vec4(Position, 1.0);
    vs_out.TexCoord = TexCoord;
}