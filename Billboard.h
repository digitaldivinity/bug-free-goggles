#ifndef KORCHAGIN_BILLBOARDS
#define KORCHAGIN_BILLBOARDS

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <iterator>
#include <map>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <chrono>

bool STOPBILLBOARDS=false;

class Billboard{
	glm::vec3 shift;
	GLfloat lifetime;
	GLfloat timepass;
	public:
	Billboard(){
		lifetime=10;
		timepass=0;
		shift=glm::vec3(0);
	}
	void Draw(GLuint Shader,glm::vec3 & Position){
		glm:: vec3 buf=Position+shift;
		GLfloat fade=1-(timepass/lifetime);
		glUniform1f(glGetUniformLocation(Shader,"Fade"),fade);
		glUniform3f(glGetUniformLocation(Shader,"shift"),buf.x,buf.y,buf.z);
		glDrawArrays(GL_POINTS,0,1);
	}
	void Update(){
		if (STOPBILLBOARDS) return;
		int buf=rand()%5;
		if (buf<3) return;
		buf=rand()%30;
		if (buf!=0) timepass+=1/buf;
		if (timepass>lifetime) rewind();
		buf=rand()%3-1;
		if (buf!=0) shift.x+=1.0/(10*buf);
		buf=rand()%3-1;
		if (buf!=0) shift.y+=1.0/(10*buf);
		buf=rand()%3-1;
		if (buf!=0) shift.z+=1.0/(10*buf);
	}
	void rewind(){
		timepass=0;
		shift=glm::vec3(0);
	}
	void normalize(){
		float buf=sqrt(shift.x*shift.x+shift.y*shift.y+shift.z*shift.z)/2;
		shift/=buf;
	}
	glm::vec3 RetPosition(glm::vec3 & Position){
		return shift+Position;
	}
};

class BillboardList{
	std::vector<Billboard> lst;
	public:
	BillboardList(GLuint & Tex,const int Amount,GLuint & Shader,GLuint & VAO):
	mShader(Shader),mVAO(VAO),mTexture(Tex)
	{
		
		for (int i =0 ;i<Amount;i++) lst.push_back(Billboard());

	}
	void Draw(glm::mat4x4 & mvp,  glm::vec3 & CamPosition, glm::vec3 & Position)
	{
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUseProgram(mShader);
		glBindVertexArray(mVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,mTexture);
		glUniform1i(glGetUniformLocation(mShader,"ColorMap"),0);
		glUniformMatrix4fv(glGetUniformLocation(mShader,"mvp"),1,GL_FALSE,&mvp[0][0]);
		glUniform3f(glGetUniformLocation(mShader,"CamPosition"),CamPosition.x,CamPosition.y,CamPosition.z);
		for (int i=0;i<lst.size();i++) lst[i].Update();
		std::map<float, Billboard> sorted;
		for (int i = 0; i < lst.size(); i++)
		{
			float distance = glm::length(CamPosition - lst[i].RetPosition(Position));
			sorted[distance] = lst[i];//sorted.insert(std::pair<float,glm::vec3>(distance,lst[i]);
		}
		for (auto it=sorted.rbegin();it!=sorted.rend();it++) it->second.Draw(mShader,Position);
		glBindVertexArray(0);
		glDisable(GL_BLEND);
	}
	private:
	GLuint & mTexture;
	GLuint & mVAO;
	GLuint & mShader;
};

#endif
