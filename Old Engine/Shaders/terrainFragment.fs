#version 330 core
out vec4 FragColor;

in vec2 texCoords;
in vec3 Normal;
in vec3 fragPos;
in float visibility;
in vec2 pass_texcoords;

uniform vec3 viewPos;

uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform vec3 skyColor;

uniform sampler2D backgroundTexture;
uniform sampler2D rBlend;
uniform sampler2D gBlend;
uniform sampler2D bBlend;
uniform sampler2D BlendMap;

uniform sampler2D defaultNormal;

uniform vec3 lightPos[30];
uniform vec3 lightColors[30];
uniform vec3 sunDir;
uniform vec3 sunColor;
uniform int nLights;


const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float alpha)
{
	float a2 = alpha*alpha;
	float nom = a2;
	
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH *NdotH;

	float denom = PI * (NdotH2 * (a2-1.0) + 1.0) * (NdotH2 * (a2-1.0) + 1.0);
	return nom/denom;
}

float GeometrySchlickGGX(vec3 N , vec3 V, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

	float NdotV = max(dot(N, V) , 0.0);
	float nom = NdotV;

	float denom = NdotV * (1.0 - k) + k;

	return nom/denom;
}

float GeometrySmith(vec3 N , vec3 V, vec3 L, float k)
{
	float ggx1 = GeometrySchlickGGX(N , V , k);
	float ggx2 = GeometrySchlickGGX(N , L ,k);

	return ggx1 *ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 getNormalFromMap(bool test)
{
     vec3 tangentNormal;
	if(test == true)
	{
        tangentNormal = texture(normalMap, texCoords).xyz * 2.0 - 1.0;
	}
	else{
		 tangentNormal = texture(defaultNormal, texCoords).xyz * 2.0 - 1.0;
	}

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(texCoords);
    vec2 st2 = dFdy(texCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}



void main()
{
    vec3 N;
	vec4 blendmaptexture = texture(BlendMap, pass_texcoords);
	float backgroundAmt = 1-(blendmaptexture.r + blendmaptexture.g + blendmaptexture.b); 
	vec4 backgroundTextureColor = texture(backgroundTexture, texCoords) * backgroundAmt;
	vec4 rBlendTexture = texture(rBlend, texCoords) * blendmaptexture.r;
	vec4 gBlendTexture = texture(gBlend, texCoords) * blendmaptexture.g;
	vec4 bBlendTexture = texture(bBlend, texCoords) * blendmaptexture.b;


		N = getNormalFromMap(true);
		


	vec4 totalColor = (backgroundTextureColor + rBlendTexture +gBlendTexture +bBlendTexture);

     vec3 albedo = pow(totalColor.rgb , vec3(2.2));
	float metallic = texture(metallicMap, texCoords).r;
	float roughness = texture(roughnessMap, texCoords).r;
	float ao = texture(aoMap, texCoords).r;

	vec3 V = normalize(viewPos - fragPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo , metallic);

	vec3 Lo= vec3(0.0);


			//calculate directional light

			vec3 L = normalize(-sunDir);
			vec3 H = normalize(V+L);


			vec3 radiance = sunColor ; // add * attenuation for point lights 


			//Cook torrance BRDF
			float D = DistributionGGX(N , H, roughness);
			vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
			float G = GeometrySmith(N , V, L , roughness);

			vec3 nom = D * G * F;
			float denom = 4*max(dot(N , V), 0.0) * max(dot(N , L), 0.0) + 0.001;
			  vec3 specular = nom / denom;


			vec3 kS = F;
			vec3 kD = vec3(1.0) - kS;

			 kD *= 1.0 - metallic;	  

			 float NdotL = max(dot(N, L), 0.0); 

			 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

	   vec3 ambient = vec3(0.03) * albedo * ao;
    
        vec3 color = ambient + Lo;

        // HDR tonemapping
        color = color / (color + vec3(1.0));
        // gamma correct
        color = pow(color, vec3(1.0/2.2)); 

         FragColor = vec4(color, 1.0);
	
	FragColor = mix(vec4(skyColor, 1.0), FragColor,   visibility);
}
