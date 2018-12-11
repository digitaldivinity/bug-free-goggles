#version 330 core

in vec2 Texture;
in vec3 Normal;
in vec3 Position;
out vec4 color;

uniform sampler2D sampler;

void main(){
	color = texture(sampler,Texture);
}
