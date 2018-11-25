#version 330

in vec4 OutColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D InTexture;

void main()
{
	vec4 sampled = texture(InTexture, TexCoord);
	FragColor = OutColor * sampled;
}