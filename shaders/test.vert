#version 330 core

in vec3 position;
in vec2 texture;
in vec3 normal;

out vec3 Position;
out vec2 Texture;
out vec3 Normal;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 nm;

void main() {
	Position=position;
	Texture=texture;
	Normal=normal;
	gl_Position = mvp * vec4(position,1);
}
