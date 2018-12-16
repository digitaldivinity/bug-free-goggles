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
uniform mat4 m;


void main() {
	Position=vec3(m*vec4(position,1));
	Texture=texture;
	Normal=nm*normal;
	gl_Position = mvp * vec4(position,1);
}
