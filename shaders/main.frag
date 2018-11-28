#version 330 core

in vec3 cubeVertex;
in vec2 tex;
out vec4 color;

uniform sampler2D sampler;
uniform float time;
void main() {
	/*
	color = vec4(cubeVertex + vec3(0.5,0.5,0.5),0);
	
	color = color * abs(sin(time));
	*/
	color = texture(sampler,tex)*abs(sin(time));
}

