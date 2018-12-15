#version 330 core

in vec3 Position;
in vec2 Texture;
in vec3 Normal;

uniform sampler2D PostMap;
out vec4 color;
uniform int Effect;


void main(){
	color = texture(PostMap,Texture);
	if (Effect==1) color = 1-color;
	if (Effect==2) color = color * vec4(1,0.8,0.2,1);
}
