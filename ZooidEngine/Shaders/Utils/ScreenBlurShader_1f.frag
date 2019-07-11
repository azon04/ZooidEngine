/**
* SSAO Fragment Shader for OPENGL
* by: Ahmad Fauzan
**/

#version 430
out float fColor;

uniform sampler2D inputTexture;

in VS_OUT
{
    vec2 TexCoord;
} fs_in;

// Configurable
const int blurRange = 4;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(inputTexture, 0));
    float result = 0.0;
    int startRange = -blurRange / 2;
    int endRange = (blurRange + 1) / 2;
    for(int x = startRange; x < endRange; ++x)
    {
        for(int y = startRange; y < endRange; ++y)
        {
            vec2 texelOffset = texelSize * vec2(float(x), float(y));
            result += texture(inputTexture, fs_in.TexCoord + texelOffset).r;
        }
    }
    fColor = result / float(blurRange * blurRange);
}