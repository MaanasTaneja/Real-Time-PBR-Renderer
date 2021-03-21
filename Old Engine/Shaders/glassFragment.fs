#version 400 core
out vec4 fragColor;

in vec2 texCoords;
in vec3 Normal;
in vec3 fragPos;

uniform samplerCube prefilterMap;
uniform vec3 viewPos;
uniform int roughness;
uniform float metallic;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	float ratio = 1.0f/1.5f;

	float transparency = 1.0f - 0.3f;

	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(fragPos - viewPos);
	vec3 reflectDir = reflect(viewDir, normal); 
	vec3 refractDir = refract(viewDir, normal, ratio);
	
	vec3 reflectColor = textureLod(prefilterMap, reflectDir, roughness).rgb;
	vec3 refractColor = textureLod(prefilterMap, refractDir, roughness).rgb;

	refractColor = pow(refractColor, vec3(1.0/2.2));
	reflectColor = pow(reflectColor, vec3(1.0/2.2));

	fragColor= vec4(mix(reflectColor, refractColor, transparency) , 1.0f);
}

