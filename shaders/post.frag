#version 330 core

in vec3 Position;
in vec2 Texture;
in vec3 Normal;

uniform sampler2D PostMap;
out vec4 color;
uniform int Effect;


void main(){
	color = texture(PostMap,Texture);
	if (Effect==1) color = 1-color;
	if (Effect==2) color = color * vec4(1,0.8,0.2,1);
	if (Effect>2) {
		const float offset =1.0/300;
		vec2 offsets[9] = vec2[](
			vec2(-offset,  offset),
			vec2( 0.0f,    offset), 
			vec2( offset,  offset),
			vec2(-offset,  0.0f), 
			vec2( 0.0f,    0.0f),
			vec2( offset,  0.0f),
			vec2(-offset, -offset),
			vec2( 0.0f,   -offset),
			vec2( offset, -offset)  
		);
		if (Effect==3){
			float kernel[9] = float[](
				-1, -1, -1,
				-1,  9, -1,
				-1, -1, -1
			);
			
			vec3 sampleTex[9];
			for(int i = 0; i < 9; i++)
			{
				sampleTex[i] = vec3(texture(PostMap, Texture.st + offsets[i]));
			}
			vec3 col = vec3(0.0);
			for(int i = 0; i < 9; i++)
				col += sampleTex[i] * kernel[i];
			
			color=vec4(col,1);
		}
		if (Effect==4){
			float kernel[9] = float[](
				1.0 / 16, 2.0 / 16, 1.0 / 16,
				2.0 / 16, 4.0 / 16, 2.0 / 16,
				1.0 / 16, 2.0 / 16, 1.0 / 16  
			);
			vec3 sampleTex[9];
			for(int i = 0; i < 9; i++)
			{ 
				sampleTex[i] = vec3(texture(PostMap, Texture.st + offsets[i]));
			}
			vec3 col = vec3(0.0);
			for(int i = 0; i < 9; i++)
				col += sampleTex[i] * kernel[i];
			
			color=vec4(col,1);
		}
		if (Effect==5){
			float kernel[9] = float[](
				1, 1, 1,
				1,-8, 1,
				1, 1, 1
			);
			
			vec3 sampleTex[9];
			for(int i = 0; i < 9; i++)
			{
				sampleTex[i] = vec3(texture(PostMap, Texture.st + offsets[i]));
			}
			vec3 col = vec3(0.0);
			for(int i = 0; i < 9; i++)
				col += sampleTex[i] * kernel[i];
			
			color=vec4(col,1);
		}
	}
}
