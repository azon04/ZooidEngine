#version 330
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec4 color;

out vec4 OutColor;
out vec2 TexCoord;

uniform vec2 screenDimension;

void main()
{
	vec3 pos = position;
	pos.x = 2.0 * (pos.x / screenDimension.x) - 1.0;
	pos.y = 1.0 - 2.0 * (pos.y / screenDimension.y);
	pos.z = 1.0 - pos.z;

	gl_Position = vec4( pos, 1.0 );
	OutColor = color;
	TexCoord = texCoord;
}