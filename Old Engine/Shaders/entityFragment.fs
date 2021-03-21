#version 450 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 brightColor;

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
uniform bool extra;

uniform float extraRough;
uniform float extraMetal;

uniform samplerCube envMap;

uniform vec3 lightPos[30];
uniform vec3 lightColors[30];
uniform vec3 areaLightPositions[10];
uniform int nAreaLights;
uniform int nLights;
uniform bool pMAP;
uniform vec3 sunDir;
uniform vec3 sunColor;

uniform float area_LineP1;
uniform float area_LineP2;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float alpha);
float GeometrySchlickGGX(vec3 N , vec3 V, float roughness);
float GeometrySmith(vec3 N , vec3 V, vec3 L, float k);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);
vec3 cookTorrance(vec3 N, vec3 H , vec3 ViewDirection, float roughness, vec3 L, float cosTheta, vec3 F0);
vec3 pointLight(vec3 lightPosition , vec3 lightColor , vec3 fragPos, vec3 N ,vec3 V,  float roughness, vec3 albedo, float metallic , vec3 F0);
vec3 directionalLight(vec3 N ,vec3 V,  float roughness, vec3 albedo, float metallic , vec3 F0);
float lambertDiffuse(vec3 lightDirection , vec3 Normal);
float area_sphere(float radius, vec3 position , vec3 N);
vec3 getNormalFromMap();

vec3 acesFilm(const vec3 x) {
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;
    return clamp((x * (a * x + b)) / (x * (c * x + d ) + e), 0.0, 1.0);
}

void main()
{	

	vec3 albedo = pow(texture(albedoMap, texCoords).rgb , vec3(2.2));
	float metallic = texture(roughnessMap, texCoords).r;
	float roughness = texture(roughnessMap, texCoords).g;
	float ao = texture(aoMap, texCoords).r;
	vec3 N = getNormalFromMap();


	if(extraMetal != 0.0f || roughness == 0.0f)
	{
	    	roughness = 0.8;
	metallic = 0.2;
	}

	
	vec3 V = normalize(viewPos - fragPos);  //view directions
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo , metallic);

	vec3 R = reflect(-V, N); 
	vec3 Lo= vec3(0.0);


	//Point Lights
	for(int i =0;i<nLights+1;i++)
      {
		 Lo += pointLight(lightPos[i], lightColors[i], fragPos, N, V, roughness, albedo, metallic, F0) ;
	  }

	 
	//Image based lighting
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse      = irradiance * albedo;
    
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);
    vec3 ambient = (kD * diffuse + specular);
  
    vec3 color = (ambient * 1.5) + Lo ;


	//Color correction 
	color = acesFilm(color);
  
    FragColor = vec4(color , 1.0f);

} 


// Standard lighting models

// This includes - Point lights
//               - Directional lights

vec3 pointLight(vec3 lightPosition , vec3 lightColor , vec3 fragPos, vec3 N ,vec3 V,  float roughness, vec3 albedo, float metallic , vec3 F0)
{
        	vec3 L = normalize(lightPosition - fragPos);
			vec3 H = normalize(V+L);

			float distance = length(lightPosition - fragPos);  //gets distance from light of pixel being shaded
			float attenuation = 1.0/(distance * distance); //Inverse square Law (gets attenuation based on distance)
			//can use quadratic attenuation but nor physically accurate

			vec3 radiance = lightColor   * attenuation; // add * attenuation for point lights 

			vec3 specular = cookTorrance(N , H, V, roughness, L, max(dot(N, V), 0.0) , F0);


			vec3 kS = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
			vec3 kD = vec3(1.0) - kS;

			 kD *= 1.0 - metallic;	  

			 float NdotL = max(dot(N, L), 0.0); 

			 return (kD * albedo / PI + specular) * radiance * NdotL ;
}


// Cook torrance BRDF assembly helper functions

// Normal distribuition - GGX 
// Geometry self shadwoing - Smiths method
// Fresnel term - fresnel Schlick

float DistributionGGX(vec3 N, vec3 H, float alpha)
{
	float a2 = alpha*alpha;
	float nom = a2;
	
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH *NdotH;

	float denom = PI * (NdotH2 * (a2-1.0) + 1.0) * (NdotH2 * (a2-1.0) + 1.0);
	return nom/denom;
}

vec3 cookTorrance(vec3 N, vec3 H , vec3 ViewDirection, float roughness, vec3 L, float cosTheta, vec3 F0)
{
	//Numerator consits of DGF term DistributionGGX , geometric shadwoing , fresnelSchlick
	float D = DistributionGGX(N , H, roughness);
	float G = GeometrySmith(N , ViewDirection , L , roughness);
	vec3 F = fresnelSchlickRoughness(cosTheta, F0, roughness);

	vec3 Numerator = D * G * F;

	float Denominator = 4 * max(dot(N , ViewDirection), 0.0f) * max(dot(N , L) , 0.0f) + 0.001;

	return Numerator / max(Denominator, 0.001);

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

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}   

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, texCoords).xyz * 2.0 - 1.0;

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


//Area Lights , based on Frostbites implementation using Representative Point

float lambertDiffuse_(vec3 lightDir , vec3 N)
{
	float diffuse = max(dot(N, lightDir), 0.0f);
	return diffuse;
}


float area_sphere(float radius, vec3 position , vec3 N)
{
	vec3 lightDir_UN = (position - fragPos);
	vec3 lightDir_N = normalize(lightDir_UN);
	float sqrDist = dot(lightDir_UN, lightDir_UN);

	float Beta = acos(dot(N, lightDir_N));
	float H = sqrt(sqrDist);
	float h = H/radius;
	float x = sqrt(h * h -1);
	float y = -x * ( 1 / tan(Beta));

	float illuminance = 0.0f;
	if(h*cos(Beta) > 1)
		illuminance = cos(Beta) / (h*h);

	else
	{
		illuminance = (1/(PI * h * h)) * (cos(Beta) * acos(y) - x * sin(Beta) * sqrt(1 - y *y)) +
		(1 / PI) * atan(sin(Beta) * sqrt(1 -y *y )/x);
	}
	illuminance *=PI;

	float distance = length(position - fragPos);
	float attenuation = 1/(distance * distance);
	illuminance *= attenuation;
	return illuminance;

}