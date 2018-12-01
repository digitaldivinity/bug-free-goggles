#version 330 core

in vec2 Texture;
in vec3 Normal;
in vec3 Position;

//in vec3 nLightDirection;
//in vec3 nNormal;

out vec4 color;

//uniform mat4 mvp;
//uniform mat4 mv;
uniform vec3 camPosition;
uniform mat3 nm;
uniform mat4 m;
uniform vec4 LightPosition=vec4(0,0,0,1);
uniform sampler2D sampler;

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
	color = vec4(ambient+diffuse+specular,1);//*texture(sampler,Texture);
}
