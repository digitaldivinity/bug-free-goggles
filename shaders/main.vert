#version 330 core

in vec3 position;
in vec2 texture;
in vec3 normal;

out vec3 Position;
out vec2 Texture;
out vec3 Normal;

//out vec3 nLightDirection;
//out vec3 nNormal;

uniform mat4 mvp;
uniform mat4 mv;
uniform mat3 nm;
//uniform vec3 LightPosition=vec3(0,0,0);


void main() {
	Position=position;
	Texture=texture;
	Normal=normal;
	gl_Position = mvp * vec4(position,1);
}
