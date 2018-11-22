#version 330 core

in vec3 cubeVertex;

out vec4 color;

uniform float time;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 modl;
void main() {
	color = vec4(cubeVertex + vec3(0.5,0.5,0.5),0);
	
	color = vec4(sin(time),cos(time),cos(time),1);
	color = abs(color);
	
}
