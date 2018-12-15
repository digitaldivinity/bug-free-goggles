#version 330 core
in vec3 position;
in vec2 texture;
in vec3 normal;

out vec3 Position;
out vec2 Texture;
out vec3 Normal;

uniform mat4 mvp;

void main(){
	Position=position;
	Texture=texture;
	Normal=normal;
	gl_Position=vec4(position.x,position.y,0,1);
}
