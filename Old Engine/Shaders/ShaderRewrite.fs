#version 330 core


struct PointLight {
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;
in vec4 FragPosLightSpace;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool gamma;
//uniform vec3 lightColor;
uniform int lightingModel;
uniform sampler2D shadowMap;
uniform float far_plane;
uniform samplerCube depthMap;

uniform int numoflights;
uniform PointLight lights[1];

float ShadowCalculation(vec4 fragPosLightSpace , vec3 normal, vec3 lightDir)
{

		vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
		 projCoords = projCoords * 0.5 + 0.5;

		 if(projCoords.z>1.0)
		 {
		 	 projCoords.z=1.0;
		 }
		float closestDepth = texture(shadowMap, projCoords.xy).r;
		float currentDepth = projCoords.z;  

		float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  
  /*
		if(closestDepth < currentDepth - bias)
		{
			shadow = 1.0;
		}
		else
		{
			shadow = 0.0;

		}

		*/
		  float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;

}

float ShadowCalculationPoint(vec3 fragPos)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fragPos - lightPos;
    // ise the fragment to light vector to sample from the depth map    
    float closestDepth = texture(depthMap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}



vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)
{

      vec3 specularTexture = texture(texture2, TexCoords).rgb;
      vec3 lightspec = glm::vec3(1.0f, 1.0f, 1.0f);



    // diffuse
    vec3 lightDir = normalize(lightPos - fragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fragPos);
   
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * specularTexture * lightspec;    
    // simple attenuation
  
    
    return diffuse + specular;
}

void main()
{

 vec3 lightColor = vec3(0.3);
	 vec3 color = texture(texture1, TexCoords).rgb;
    vec3 lighting ;
	vec3 temp ;
	vec3 ambient=  0.05 * color;
   

     lighting = BlinnPhong(normalize(Normal), FragPos, lights[0].position , color) +ambient ;

         // temp = pow(temp, vec3(1.0/2.2));  //gamma correction
		//float shadow = ShadowCalculation(FragPos);
	   //lighting = (ambient + (1.0 - shadow)) * lighting * color;    

    FragColor = vec4(lighting, 1.0);
}