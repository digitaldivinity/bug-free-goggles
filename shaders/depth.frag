#version 330 core
in vec4 FragPos;
uniform vec3 LightPos;
uniform float far_plane;

void main(){
	
	float lightDistance=length(FragPos.xyz-LightPos);
	lightDistance/=far_plane;
	gl_FragDepth=lightDistance;
	
}
