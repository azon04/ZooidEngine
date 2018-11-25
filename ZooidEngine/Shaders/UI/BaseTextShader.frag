#version 330

in vec4 OutColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D InTexture;

void main()
{
	vec4 sampled = vec4(texture(InTexture, TexCoord).r);
	if(sampled.a <= 0.0) { discard; }
	FragColor = OutColor * sampled;
}