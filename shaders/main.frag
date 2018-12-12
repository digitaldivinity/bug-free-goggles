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
uniform Material material={vec3(0.2,0.2,0.2),vec3(0.7,0.7,0.7),vec3(1,1,1),200};
struct Light{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform Light light={vec3(1,1,1),vec3(1,1,1),vec3(1,1,1)};

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   
float ShadowCalculation(vec3 FragPos){
	vec3 fragToLight = FragPos-vec3(0,0,0);
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
	vec3 p = Position;
	vec3 l=normalize(vec3(LightPosition)-p);
	vec3 n=normalize(Normal);
	vec3 v=normalize(camPosition-p);
	vec3 r= reflect(-v,n);
	float DiffStr=max(dot(n,l),0);
	float SpecStr=pow(max(dot(l,r),0),material.shininess);
	vec3 ambient=light.ambient*material.ambient;
	vec3 diffuse=light.diffuse*(DiffStr*material.diffuse);
	vec3 specular = light.specular*(SpecStr*material.specular);
	float shadow = ShadowCalculation(Position);
	color = vec4(ambient+(1-shadow)*(diffuse+specular),1);
}
