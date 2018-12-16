#version 330 core
in vec3 position;

out vec3 TexCoords;

uniform mat4 mvp;

void main()
{
	TexCoords = vec3(position.x,position.y,position.z);
	gl_Position = mvp * vec4(position, 1.0);
}
