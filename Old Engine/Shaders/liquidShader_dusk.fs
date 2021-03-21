#version 330 core
out vec4 FragColor;

in vec2 texCoords;
in vec3 Normal;
in vec4 FragPos;
in float noise;

uniform sampler2D liquidTexture;
uniform float currentTime;
uniform vec3 camPos;
uniform vec3 flowDirection;

uniform float alpha;
uniform bool additiveBlend;


float sineGlow()
{
	float sin = sin(currentTime *1.5);
	return clamp(sin , 0.4, 1.2f); 
}

void main()
{
	vec3 direction = vec3(1.0f, 0.0f, 1.0f) * 10;
	vec3 color = texture(liquidTexture, vec2(texCoords.x + (-direction.x) ,texCoords.y + (-direction.z))).rgb;

	if(additiveBlend == true)
	{
	   color += color ;
	}

  gl_FragColor = vec4( color.rgb, alpha);
}