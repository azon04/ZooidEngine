#version 330

in vec4 OutColor;
in vec2 TexCoord;
in float Roundness;
in vec2 ShapeDimension;

out vec4 FragColor;

void main()
{
	vec2 coords = TexCoord * ShapeDimension;
	bool toDiscard = (coords.x < Roundness && coords.y < Roundness && length(vec2(Roundness) - coords) > Roundness ) ||
		( coords.x > ShapeDimension.x - Roundness && coords.y < Roundness && length(vec2(ShapeDimension.x - Roundness, Roundness) - coords) > Roundness ) ||
		( coords.y > ShapeDimension.y - Roundness && coords.x < Roundness && length(vec2(Roundness, ShapeDimension.y - Roundness) - coords) > Roundness ) ||
		( coords.x > ShapeDimension.x - Roundness && coords.y > ShapeDimension.y - Roundness && length(vec2(ShapeDimension.x - Roundness, ShapeDimension.y - Roundness) - coords) > Roundness );
	
	float alpha = toDiscard ? 0.0 : 1.0;
	
	if( toDiscard ) { discard; }

	FragColor = OutColor * vec4( 1.0f, 1.0f, 1.0f, alpha );	
}