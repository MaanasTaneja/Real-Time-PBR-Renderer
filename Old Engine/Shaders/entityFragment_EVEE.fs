#version 330 core

out vec4 FragColor;
in vec2 texCoords;
in vec3 Normal;
in vec3 fragPos;
in float visibility;

uniform vec3 viewPos;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;
uniform vec3 skyColor;

const float PI = 3.14159265359;

uniform vec3 lightPos[30];
uniform vec3 lightColors[30];
uniform int nLights;

void main()
{	

	vec3 albedo = pow(texture(albedoMap, texCoords).rgb , vec3(2.2));
	float metallic = texture(roughnessMap, texCoords).r;
	float roughness = texture(roughnessMap, texCoords).g;
	float ao = texture(aoMap, texCoords).r;
	vec3 N = normalize(Normal);
	vec3 V = normalize(viewPos - fragPos);  //view directions
	vec3 Lo = vec3(0.0f);

	for(int i = 0; i<nLights ; i++)
	{
	   vec3 lightDirection = normalize(lightPos[i] - fragPos);
	   float diffuse = max(dot(lightDirection, N), 0.0f);
	   Lo = lightColors[i] * diffuse;

	}

	vec3 ambient = 0.2 * albedo;
	vec3 diffuseColor = albedo * Lo;
	vec3 finalColor = diffuseColor + ambient;

	FragColor = vec4(finalColor , 1.0f);

} 
