#version 330 core

in vec3 position;
in vec2 texture;
in vec3 normal;

uniform mat4 model;
void main(){
	gl_Position=model*vec4(position,1);
}
