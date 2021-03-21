#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec2 textureCoords;
out vec2 texCoords1;
out vec2 texCoords2;
out float blend;

uniform vec2 texOffset1;
uniform vec2 texOffset2;
uniform vec2 texCoordInfo;

void main()
{
	textureCoords = TexCoords;

	gl_Position = projection * view * model * vec4(aPos, 1.0f);
}