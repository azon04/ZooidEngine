/***
* Default Screen Quad Fragment Shader for OPENGL Rendering
* Ahmad Fauzan
***/

#version 430

out vec4 fColor;
in vec2 vsTexCoord;

uniform sampler2D InTexture;

void main()
{
    vec4 color = texture(InTexture, vsTexCoord);
    if( color.a <= 0.0 ) { discard; }
    fColor = color;
}