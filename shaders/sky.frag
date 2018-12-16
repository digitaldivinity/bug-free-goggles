#version 330 core
out vec4 color;

in vec3 TexCoords;

uniform samplerCube SkyBox;

void main()
{
	color = texture(SkyBox, TexCoords);
}
