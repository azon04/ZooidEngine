#version 330

in vec4 OutColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D InTexture;

void main()
{
	float sampled = texture(InTexture, TexCoord).r;
	if(sampled <= 0.0) { discard; }
	FragColor = vec4(OutColor.xyz, OutColor.w * sampled);
}