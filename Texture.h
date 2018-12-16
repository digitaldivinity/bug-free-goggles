#ifndef KORCHAGIN_TEXTURE_CLASS
#define KORCHAGIN_TEXTURE_CLASS
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <SOIL/SOIL.h>
class Texture{
	int Width;
	int Height;
	int channels;
	unsigned char * ptr;
	public:
	Texture(const char * path,GLuint format){
		if (format==GL_RGBA)
		ptr=SOIL_load_image(path, &Width, &Height, &channels, SOIL_LOAD_RGBA);
		else if (format==GL_RGB)
		ptr=SOIL_load_image(path, &Width, &Height, &channels, SOIL_LOAD_RGB);
	}
	~Texture(){
		SOIL_free_image_data(ptr);
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
