#version 450 core
out vec4 FragColor;

in vec2 textureCoords;
in vec2 texCoords1;
in vec2 texCoords2;
in float blend;

uniform sampler2D particleTexture;
uniform bool randomizeColors;
uniform vec3 randomColor;
uniform vec3 setColor;

uniform bool hasTexture;
uniform bool hasForceColor;

void main()
{
	vec3 color1 = texture(particleTexture, texCoords1).rgb;
	vec3 color2 = texture(particleTexture, texCoords2).rgb;
		
	vec4 final;
	
		if(randomizeColors == true)
	{
		final = vec4(randomColor, 1.0f);
	}
	else{
		final = vec4(1.0f);
	}
	

	if(hasTexture == true)
	{
	   final = texture(particleTexture, textureCoords);
	   	if(final.a < 0.1f)
	    {
		   discard;
	    }
	}
	


	final =+ final;
	FragColor = final;
}