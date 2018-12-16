#version 330 core

in vec2 Texture;
in vec3 Normal;
in vec3 Position;

//in vec3 nLightDirection;
//in vec3 nNormal;

out vec4 color;

//blinn-fong model
uniform vec3 camPosition;
uniform mat3 nm;
uniform mat4 m;
uniform vec3 LightPosition;
uniform sampler2D normalMap;

uniform sampler2D sampler;
//shadow mapping
uniform samplerCube depthMap;
uniform float farPlane=500;
//uniform mat4 LightSpace;
//in vec4 FragPosLightSpace;

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material={vec3(0.1,0.1,0.1),vec3(0.7,0.7,0.7),vec3(1,1,1),200};
struct Light{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light={vec3(0.1,0.1,0.1),vec3(0.7,0.7,0.7),vec3(1,1,1)};

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   
float ShadowCalculation(vec3 FragPos){
	vec3 fragToLight = FragPos-LightPosition;
	float closestDepth=texture(depthMap,fragToLight).r;
	closestDepth*=farPlane;
	float currentDepth = length(fragToLight);
	
	float shadow = 0.0;
	float bias = 0.15;
	int samples = 20;
	float viewDistance = length(camPosition - FragPos);
	float diskRadius = 0.05;
	for(int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= farPlane;   // Undo mapping [0;1]
		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples); 
	if (shadow>1.0) shadow=1.0;
	return shadow;
}


void main()
{
	//
	vec3 deltaVec=vec3(0,0,1)-normalize(texture(normalMap,Texture).rgb*2-1);
	vec3 n = normalize(Normal-nm*deltaVec);
	//
	vec3 p = Position;
	vec3 l=normalize(LightPosition-p);
	vec3 v=normalize(camPosition-p);
	vec3 h=normalize(l+v);

	float cosNL = max(dot(n, l), 0.0);
	float cosNV = max(dot(n, v), 0.0);
	float cosNH = max(dot(n, h), 1.0e-7);
	float cosVH = max(dot(v, h), 0.0);

	float geometric = 2.0 * cosNH/ cosVH;
	geometric = min(1.0, geometric * min(cosNV, cosNL));

	float roughnessVal=0.15;
	float roughness2 = roughnessVal * roughnessVal;
	float cosNH2 = cosNH * cosNH;
	float cosNH2r = 1.0 / (cosNH2 * roughness2);
	float roughnessE = (cosNH2 - 1.0) * cosNH2r;
	float roughness = exp(roughnessE) * cosNH2r / (4.0 * cosNH2);
	float fresnel = 1.0 / (1.0 + cosNV);
	float Rs = min(1.0, (fresnel * geometric * roughness) / (cosNV * cosNL + 1.0e-7));
	float shadow = ShadowCalculation(p);
	vec3 precolor =  cosNL * (light.diffuse+ light.specular* Rs);

	color = vec4(light.ambient + (1-shadow) * precolor, 1.0);
	//color = vec4(ambient+(1-shadow)*(diffuse+specular),1);
}
