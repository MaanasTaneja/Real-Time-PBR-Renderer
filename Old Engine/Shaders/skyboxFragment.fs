#version 400

out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube skybox;
uniform vec3 fogColor;

const float lowerLimit = -0.0;
const float upperLimit = 0.5f;

void main()
{    
	  vec3 finalColor  = texture(skybox, TexCoords).rgb;
	float factor = (TexCoords.y - lowerLimit) / (upperLimit - lowerLimit);
	factor = clamp(factor, 0.0, 1.0);
  
  // finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(1.0/4.2)); 

  FragColor = vec4(finalColor, 1.0f);
	//FragColor = mix(vec4(fogColor, 1.0f) , finalColor, factor);
}