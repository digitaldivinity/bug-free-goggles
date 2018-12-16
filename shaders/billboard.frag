#version 330 core

in vec2 Texture;
out vec4 color;

uniform sampler2D ColorMap;
uniform float Fade;
void main(){
	
	color = texture2D(ColorMap,Texture)*Fade;
	if (color.w<=0.1) discard;
} 
