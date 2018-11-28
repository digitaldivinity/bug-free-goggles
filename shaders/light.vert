#version 330 core

in vec3 modelPos;
out vec3 cubeVertex;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 modl;
//uniform float time;

void main() {
	cubeVertex = modelPos;
	gl_Position = proj * view * modl * vec4(modelPos,1.0f);
}
