#version 330 core

in vec3 modelPos;
in vec2 texPos;
in vec3 colorPos;
out vec3 cubeVertex;
out vec2 tex;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 modl;
//uniform float time;

void main() {
	tex = texPos;
	cubeVertex = modelPos;
	gl_Position = proj * view * modl * vec4(modelPos,1.0f);
}
