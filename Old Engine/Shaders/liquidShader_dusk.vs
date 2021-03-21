#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out vec3 Normal;
out vec2 texCoords;
out vec4 FragPos;
out float noise;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;

uniform float currentTime;

uniform float tiling;

uniform float waveSpeedFactor;


vec2 SineWave( vec2 p ){
    float pi = 3.14159;
    float A = 0.08;
    float w = 10.0 * pi;
    float t = 30.0*pi/180.0;
    float y = sin( w*p.x + t+currentTime*waveSpeedFactor) * A ;

	float x = sin( w*p.y + t+currentTime*waveSpeedFactor) * A ;
    return vec2(p.x+x, p.y+y);
}

void main()
{
	texCoords = aTexCoords *tiling;
	Normal = normalMatrix * aNormal;

	vec2 sinePositions = SineWave(vec2(aPos.x, aPos.z));
	gl_Position = projection * view * model * vec4(sinePositions.x,  aPos.y , sinePositions.y, 1.0f);
}