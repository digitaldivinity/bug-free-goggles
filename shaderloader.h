#ifndef KORCHAGIN_SHADER_LOADER
#define KORCHAGIN_SHADER_LOADER

//я изобретаю колесо, снова
//и оно квадратное
class ShaderLoader{
	char * src;
	long length;
	public:
	ShaderLoader(const char * path){
		FILE * file;
		if (!(file=fopen(path,"r"))) throw "Wrong file\n";
		length=0;
		while (getc(file)!=EOF) length++;
		fclose(file);
		long i=0;
		src=new char[length+1];
		fopen(path,"r");
		while (i<length) {
			src[i]=getc(file);
			i++;
		}
		src[i]='\0';
	}
	~ShaderLoader(){
		delete [] src;
	}
	//glShaderSource принимает массив строк, поэтому указатель на указатель
	char ** getr(){
		return &src;
	}
};

#endif
