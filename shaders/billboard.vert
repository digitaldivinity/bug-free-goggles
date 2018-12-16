#version 330 core

in vec3 position;

uniform vec3 shift;
void main(){
	gl_Position = vec4(position+shift,1);
}
