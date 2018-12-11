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
uniform vec4 LightPosition=vec4(0,0,0,1);
uniform sampler2D sampler;

//shadow mapping
uniform sampler2D depthMap;
uniform mat4 LightSpace;
in vec4 FragPosLightSpace;

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};
uniform Material material={vec3(0.2,0.2,0.2),vec3(0.7,0.7,0.7),vec3(1,1,1),200};
struct Light{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light={vec3(1,1,1),vec3(1,1,1),vec3(1,1,1)};

float ShadowCalculation(vec4 fragPosLightSpace){
	vec3 projCoords = fragPosLightSpace.xyz/fragPosLightSpace.w;
	//приведение к интервалу [0,1]
	projCoords=projCoords*0.5+0.5;
	//float closestDepth=texture(depthMap,projCoords.xy).r;
	float currentDepth=projCoords.z;
	//float bias=0.005;
	float bias=min(0.05*(1-dot(Normal,vec3(0,0,1))),0.005);
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(depthMap, 0);
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 25;
	//shadow = currentDepth -bias> closestDepth ? 1.0 : 0.0;
	
	return shadow;
}

void main()
{
	vec3 p = vec3(m* vec4(Position,1));
	vec3 l=normalize(vec3(LightPosition)-p);
	vec3 n=normalize(nm*Normal);
	vec3 v=normalize(camPosition-p);
	vec3 r= reflect(-v,n);
	float DiffStr=max(dot(n,l),0);
	float SpecStr=pow(max(dot(l,r),0),material.shininess);
	vec3 ambient=light.ambient*material.ambient;
	vec3 diffuse=light.diffuse*(DiffStr*material.diffuse);
	vec3 specular = light.specular*(SpecStr*material.specular);
	//color = vec4(ambient+diffuse+specular,1);
	float shadow = ShadowCalculation(FragPosLightSpace);
	color = vec4(ambient+(1-shadow)*(diffuse+specular),1);
}
