#version 400 core
out vec4 FragColor;

in vec2 texCoords;
in vec3 Normal;
in float visibility;

uniform vec3 skyColor;
uniform sampler2D reflectionTexture;


void main()
{
	vec4 reflection = texture(reflectionTexture , texCoords);
	FragColor = reflection;
}