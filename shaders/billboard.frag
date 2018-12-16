#version 330 core

in vec2 Texture;
out vec4 color;

uniform sampler2D ColorMap;

void main(){
	
	color = texture2D(ColorMap,Texture);
	//if (color.w<0.2) discard;
} 
