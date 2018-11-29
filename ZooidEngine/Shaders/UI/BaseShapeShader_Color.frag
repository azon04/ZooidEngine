#version 330

in vec4 OutColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform float roundness;
uniform vec2 shapeDimension;

void main()
{
	vec2 coords = TexCoord * shapeDimension;
	bool toDiscard = (coords.x < roundness && coords.y < roundness && length(vec2(roundness) - coords) > roundness ) ||
		( coords.x > shapeDimension.x - roundness && coords.y < roundness && length(vec2(shapeDimension.x - roundness, roundness) - coords) > roundness ) ||
		( coords.y > shapeDimension.y - roundness && coords.x < roundness && length(vec2(roundness, shapeDimension.y - roundness) - coords) > roundness ) ||
		( coords.x > shapeDimension.x - roundness && coords.y > shapeDimension.y - roundness && length(vec2(shapeDimension.x - roundness, shapeDimension.y - roundness) - coords) > roundness );
	
	float alpha = toDiscard ? 0.0 : 1.0;
	
	if( toDiscard ) { discard; }

	FragColor = OutColor * vec4( 1.0f, 1.0f, 1.0f, alpha );	
}