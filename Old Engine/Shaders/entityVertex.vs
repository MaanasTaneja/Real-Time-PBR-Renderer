#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 texCoords;
out vec3 Normal;
out float visibility;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;
uniform mat3 normalMatrix;

const float density = 0.012;
const float gradient = 6.0;

uniform vec4 clipPlane;

void main()
{
   vec4 worldPos = model * vec4(aPos, 1.0);
   	gl_ClipDistance[0] = dot(worldPos, clipPlane);
    fragPos = vec3(model * vec4(aPos, 1.0));

	vec4 positionRelativeToCam  = view * worldPos ;
	Normal =  normalMatrix * aNormal;
	texCoords = aTexCoords ;
	gl_Position = projection * positionRelativeToCam;

	float distance  = length(positionRelativeToCam.xyz);

	visibility = exp(-pow((distance*density), gradient));
	visibility = clamp(visibility, 0.0, 1.0);
}