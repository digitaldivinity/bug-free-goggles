#ifndef KORCHAGIN_TEXTURE_CLASS
#define KORCHAGIN_TEXTURE_CLASS
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
class Texture{
	int Width;
	int Height;
	int channels;
	unsigned char * ptr;
	public:
	Texture(const char * path,GLuint format=GL_RGB){
		//stbi_set_flip_vertically_on_load(true); 
		if (format==GL_RGBA)
		ptr=stbi_load(path, &Width, &Height, &channels, 0);
		else if (format==GL_RGB)
		ptr=stbi_load(path, &Width, &Height, &channels, 0);
	}
	~Texture(){
		stbi_image_free(ptr);
	}
	unsigned char * get(){
		return ptr;
	}
	int getWidth(){
		return Width;
	}
	int getHeight(){
		return Height;
	}
};

#endif
