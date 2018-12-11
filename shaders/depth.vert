#version 330 core

in vec3 position;
in vec2 texture;
in vec3 normal;

uniform mat4 LightSpace;

void main(){
	gl_Position=LightSpace*vec4(position,1);//*model*vec4(position,1);
}
