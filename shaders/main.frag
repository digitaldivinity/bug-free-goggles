#version 330 core

in vec2 Texture;
in vec3 Normal;
in vec3 Position;

//in vec3 nLightDirection;
//in vec3 nNormal;

out vec4 color;

uniform vec4 ambient = vec4(0.2,0.2,0.2,1);
//uniform mat4 mvp;
//uniform mat4 mv;
uniform vec3 camPosition;
uniform mat3 nm;
uniform mat4 m;
uniform vec4 LightPosition=vec4(0,0,0,1);
uniform sampler2D sampler;
void main()
{
	vec3 p = vec3(m* vec4(Position,1));
	vec3 l=normalize(vec3(LightPosition)-p);
	vec3 n=normalize(nm*Normal);
	vec3 v=normalize(camPosition-p);
	vec3 r= reflect(-v,n);
	vec4 diffuse = vec4(0.5,0.5,0.5,1)*max(dot(n,l),0);
	vec4 specular= vec4(1,1,1,1)*pow(max(dot(l,r),0),200);
	color=(diffuse+specular+vec4(0.2,0.2,0.2,1));//*texture(sampler,Texture);
}
