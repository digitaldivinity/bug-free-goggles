#ifndef KORCHAGIN_SHADER
#define KORCHAGIN_SHADER

#include <iostream>

using namespace std;
class ShaderLoader{
	char * src;
	long length;
	public:
	ShaderLoader(const char * path){
		if (path!=NULL){
			FILE * file;
			if (!(file=fopen(path,"r"))) throw "Wrong file\n";
			length=0;
			while (getc(file)!=EOF) length++;
			fclose(file);
			long i=0;
			src=new char[length+1];
			file =fopen(path,"r");
			while (i<length) {
				src[i]=getc(file);
				i++;
			}
			src[i]='\0';
			fclose(file);
		}
		else src=NULL;
	}
	~ShaderLoader(){
		if (src!=NULL)
		delete [] src;
	}
	//glShaderSource принимает массив строк, поэтому указатель на указатель
	char ** getr(){
		return &src;
	}
};
GLuint CreateShader(const char * vert, const char * frag,const char * geom = NULL){
	GLuint program = glCreateProgram();
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geom_shader;
	if (geom!=NULL) geom_shader=glCreateShader(GL_GEOMETRY_SHADER);
	//мой загрузчик файлов в строку
	ShaderLoader vsl(vert),fsl(frag),gsl(geom);
	glShaderSource(vertex_shader, 1, vsl.getr(), NULL);
	glShaderSource(fragment_shader, 1, fsl.getr(), NULL);
	if (geom!=NULL) glShaderSource(geom_shader,1,gsl.getr(),NULL);
	// Compile The Shaders
	glCompileShader(vertex_shader);
	glCompileShader(fragment_shader);
	if (geom!=NULL) glCompileShader(geom_shader);
	// Attach The Shader Objects To The Program Object
	glAttachShader(program, vertex_shader);
	if (geom!=NULL) glAttachShader(program,geom_shader);
	glAttachShader(program, fragment_shader);
	// Link The Program Object
	glLinkProgram(program);
	//удаляем шейдеры после связывания
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	if (geom!=NULL) glDeleteShader(geom_shader);
	char log[10000];
	int log_len;
	glGetProgramInfoLog(program, sizeof(log) / sizeof(log[0]) - 1, &log_len, log);
	log[log_len] = 0;
	cout << "Shader from " << vert<< " and "<<frag<< " compile result:\n "<<log << endl;
	return program;
}
#endif
