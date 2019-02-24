#version 430
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec4 color;
layout (location = 3) in vec3 instancePos;
layout (location = 4) in vec3 dimension;
layout (location = 5) in vec4 instanceColor;
layout (location = 6) in vec4 uvDim;

out vec4 OutColor;
out vec2 TexCoord;

uniform vec2 screenDimension;

void main()
{
	vec3 pos = position * dimension + instancePos;
	pos.x = 2.0 * (( pos.x ) / screenDimension.x) - 1.0;
	pos.y = 2.0 * ((pos.y + screenDimension.y) / screenDimension.y) - 1.0;
	pos.z = 1.0 - instancePos.z;

	gl_Position = vec4( pos, 1.0 );
	OutColor = color * instanceColor;
	TexCoord = texCoord * uvDim.zw + uvDim.xy;
}