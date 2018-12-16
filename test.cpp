#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

//эксперементальное расширение glm 
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <iterator>
#include <map>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <numeric>
#include <omp.h>
#include <SOIL/SOIL.h>

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "FPScounter.h"
#include "Billboard.h"
using namespace std;

GLfloat flat_vertices[]={
	-1,1,0,0,1,0,0,1,
	-1,-1,0,0,1,0,0,0,
	1,-1,0,0,1,0,1,0,
	
	-1,1,0,0,1,0,0,1,
	1,-1,0,0,1,0,1,0,
	1,1,0,0,1,0,1,1
};
GLfloat bil_pos[]={
	0,0,0,0,0,0,0,0
};
GLfloat stars[900];


Model mdl1("models/lowPoly.obj");
Model sphere("models/sphere.obj");

Camera cam(0.1,0.5);
GLuint Width=512,Height=512;

GLuint VBO1,VBO2,VAOLightSource,FlatVBO;
GLuint VAO1,VAO2,TestVAO;
GLuint ShaderMain,ShaderLightSource,ShaderDepth,ShaderTest;
GLuint TEX1,TEX2,TEX3;
GLuint starsVAO,starsVBO;
GLuint depthMapFBO;
GLuint depthMap;
GLuint depthVAO,depthVAO2;
GLuint SHADOW_WIDTH=1024,SHADOW_HEIGHT=1024;
GLuint PostMap,PostFBO,COLOR_WIDTH=2048,COLOR_HEIGHT=2048,ShaderPost,VAOPost;
GLuint ShaderBillboard,VAOBillboard;
GLuint ShaderCook,VAO1Cook,VAO2Cook;
GLuint buf;
//GLuint mvpLoc;
GLuint Effect=0;

float xAngle = 0;
float yAngle = 0;

FPScounter FPS;

glm::mat4x4 proj;
glm::mat4x4 model;
glm::mat4x4 view;
glm::mat4x4 identity(1);
glm::mat4x4 LightSpace;
glm::vec3 light(0,0,0);
glm::mat4x4 mvp;
glm::mat4x4 mv;
glm::mat3x3 nm;

BillboardList blst(TEX2,10,ShaderBillboard,VAOBillboard);

bool init()
{
	std::srand(std::time(0));
	//make stars
	{
		double a;
		for (int i=0;i<900;i+=3){
			stars[i]=rand()%500-250;
			stars[i+1]=rand()%500-250;
			stars[i+2]=rand()%500-250;
			a=sqrt(pow(stars[i],2)+pow(stars[i+1],2)+pow(stars[i+2],2));
			stars[i]=300*stars[i]/a;
			stars[i+1]=300*stars[i+1]/a;
			stars[i+2]=300*stars[i+2]/a;
		}
	}

	glEnable(GL_DEPTH_TEST);

	ShaderMain=CreateShader("shaders/main.vert","shaders/main.frag");
	ShaderLightSource=CreateShader("shaders/light.vert","shaders/light.frag");
	ShaderDepth=CreateShader("shaders/depth.vert","shaders/depth.frag","shaders/depth.geom");
	ShaderTest=CreateShader("shaders/test.vert","shaders/test.frag");
	ShaderPost=CreateShader("shaders/post.vert","shaders/post.frag");
	ShaderBillboard=CreateShader("shaders/billboard.vert","shaders/billboard.frag","shaders/billboard.geom");
	ShaderCook=CreateShader("shaders/cook.vert","shaders/cook.frag");
	//generating VAO
	VAO1=mdl1.CreateArrays(ShaderMain);
	VAO2=sphere.CreateArrays(ShaderMain);
	VAO1Cook=mdl1.CreateArrays(ShaderCook);
	VAO2Cook=sphere.CreateArrays(ShaderCook);
	VAOLightSource=sphere.CreateArrays(ShaderLightSource);
	depthVAO=mdl1.CreateArrays(ShaderDepth);
	depthVAO2=sphere.CreateArrays(ShaderDepth);
	VAOPost=Model::CreateExternalArrays(ShaderPost,flat_vertices,6);
	VAOBillboard=Model::CreateExternalArrays(ShaderBillboard,bil_pos,1);
	//starsVAO
	glGenVertexArrays(1,&starsVAO);
	glBindVertexArray(starsVAO);
	glGenBuffers(1,&starsVBO);
	glBindBuffer(GL_ARRAY_BUFFER,starsVBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(stars),stars,GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(ShaderLightSource,"position"),
		3, GL_FLOAT, GL_FALSE, 0 , 0);
	glEnableVertexAttribArray(0);
	//unbind VAO & VBO
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);

	//GEN textures
	
	Texture tex1("textures/nm_brick.jpg",GL_RGB);
	glGenTextures(1, &TEX1);
	glBindTexture(GL_TEXTURE_2D, TEX1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex1.getWidth(),tex1.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, tex1.get());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	Texture tex2("textures/fire.png",GL_RGBA);
	glGenTextures(1, &TEX2);
	glBindTexture(GL_TEXTURE_2D, TEX2);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex2.getWidth(),tex2.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex2.get());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	Texture tex3("textures/melon.bmp",GL_RGB);
	glGenTextures(1, &TEX3);
	glBindTexture(GL_TEXTURE_2D, TEX3);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex3.getWidth(),tex3.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex3.get());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//will be uncommet
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	//буффер для карты глубины и текстура
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP,depthMap);
	for (int i =0;i<6;i++){
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,GL_DEPTH_COMPONENT,SHADOW_WIDTH,SHADOW_HEIGHT,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
		
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); 
	
	glBindFramebuffer(GL_FRAMEBUFFER,depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,depthMap,0);
	glDrawBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	//буффер для постэффектов
	GLuint depthPostMap;
	glGenFramebuffers(1, &PostFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, PostFBO);
	// create a color attachment texture
	glGenTextures(1, &PostMap);
	glBindTexture(GL_TEXTURE_2D, PostMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, COLOR_WIDTH, COLOR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, PostMap, 0);
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, COLOR_WIDTH, COLOR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glEnable(GL_DEPTH_TEST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void reshape(int w, int h)
{
	Width=w;
	Height=h;
	proj = glm::perspective(45.0f, float(w)/float(h), 1.0f, 500.0f);
	glViewport(0, 0, w, h);
	glutPostRedisplay();
}

GLfloat rotSpeed=0;

void idle(int value) {
	cam.move();
	//xAngle+=rotSpeed;
	yAngle+=rotSpeed;
	if (xAngle > 720) xAngle-=720;
	if (yAngle > 720) yAngle-=720;
	glutPostRedisplay();
	glutTimerFunc(20,idle,0);
}


glm::mat4x4 shadowTransforms[6];//vp
void display(void)
{
	//рендер карты глубины
	glCullFace(GL_FRONT);
	glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER,depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	GLfloat far=500;
	proj=glm::perspective(glm::radians(90.0f),(float)SHADOW_WIDTH/SHADOW_HEIGHT,1.0f,500.0f);
	//vp матрицы
	shadowTransforms[0]=proj * glm::lookAt(light, light + glm::vec3( 1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0));
	shadowTransforms[1]=proj * glm::lookAt(light, light + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0,-1.0, 0.0));
	shadowTransforms[2]=proj * glm::lookAt(light, light + glm::vec3( 0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
	shadowTransforms[3]=proj * glm::lookAt(light, light + glm::vec3( 0.0,-1.0, 0.0), glm::vec3(0.0, 0.0,-1.0));
	shadowTransforms[4]=proj * glm::lookAt(light, light + glm::vec3( 0.0, 0.0, 1.0), glm::vec3(0.0,-1.0, 0.0));
	shadowTransforms[5]=proj * glm::lookAt(light, light + glm::vec3( 0.0, 0.0,-1.0), glm::vec3(0.0,-1.0, 0.0));

	glUseProgram(ShaderDepth);
	glBindVertexArray(depthVAO);
	
	model = glm::rotate(glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::vec3(0.0f,0.0f,-10.0f))*
		glm::rotate(glm::radians(xAngle),glm::vec3(1,0,0));
	for (int i=0;i<6;i++){
		char buf[]="shadowMatrices[0]";
		buf[15]=i+'0';
		glUniformMatrix4fv(glGetUniformLocation(ShaderDepth,buf),1,GL_FALSE,&((shadowTransforms[i])[0][0]));
	}
	glUniform1f(glGetUniformLocation(ShaderDepth,"far_plane"),far);
	glUniform3f(glGetUniformLocation(ShaderDepth,"LightPosition"),light.x,light.y,light.z);
	glUniformMatrix4fv(glGetUniformLocation(ShaderDepth,"model"),1,GL_FALSE,&model[0][0]);
	glDrawArrays(GL_TRIANGLES,0,mdl1.getSize());
	model = glm::rotate(glm::radians(-yAngle), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::vec3(0.0f,0.0f,-20.0f))*
		glm::scale(glm::vec3(2,2,2));
	glBindVertexArray(depthVAO2);
	glUniformMatrix4fv(glGetUniformLocation(ShaderDepth,"model"),1,GL_FALSE,&model[0][0]);
	glDrawArrays(GL_TRIANGLES,0,sphere.getSize());
	model = glm::rotate(glm::radians(yAngle/2), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::vec3(0.0f,0.0f,-40.0f))*
		glm::scale(glm::vec3(3,3,3));
	glUniformMatrix4fv(glGetUniformLocation(ShaderDepth,"model"),1,GL_FALSE,&model[0][0]);
	glDrawArrays(GL_TRIANGLES,0,sphere.getSize());
	glCullFace(GL_BACK);
	
	//рендер сцены
	glViewport(0,0,COLOR_WIDTH,COLOR_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER,PostFBO);
	proj=glm::perspective(glm::radians(45.0f),(float)Width/Height,1.0f,500.0f);
	
	GLuint mvpLoc,mvLoc,nmLoc;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	view=cam.getView();
	
	mvp = proj * view;
	
	//первая
	model = glm::rotate(glm::radians(yAngle), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::vec3(0.0f,0.0f,-10.0f))*
		glm::rotate(glm::radians(xAngle),glm::vec3(1,0,0));
	mv=view*model;
	nm = glm::transpose(glm::inverse(glm::mat3x3(model)));
	mvp = proj * mv;
	glUseProgram(ShaderMain);
	glBindVertexArray(VAO1);
	glBindTexture(GL_TEXTURE_2D, TEX1);
	mvpLoc=glGetUniformLocation(ShaderMain,"mvp");
	mvLoc=glGetUniformLocation(ShaderMain,"mv");
	nmLoc=glGetUniformLocation(ShaderMain,"nm");
	GLuint mLoc=glGetUniformLocation(ShaderMain,"m");
	GLuint camposLoc=glGetUniformLocation(ShaderMain,"camPosition");
	glUniform3f(glGetUniformLocation(ShaderDepth,"LightPosition"),light.x,light.y,light.z);
	glActiveTexture(GL_TEXTURE0);//
	glBindTexture(GL_TEXTURE_CUBE_MAP,depthMap);//
	GLuint depthMapLoc=glGetUniformLocation(ShaderMain,"depthMap");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix3fv(nmLoc, 1, GL_FALSE, &nm[0][0]);
	glUniformMatrix4fv(mLoc,1,GL_FALSE,&model[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(ShaderMain,"LightSpace"),1,GL_FALSE,&LightSpace[0][0]);
	glUniform3f(camposLoc,cam.x,cam.y,cam.z);
	glBindTexture(GL_TEXTURE_CUBE_MAP,depthMap); 
	glUniform1i(glGetUniformLocation(ShaderMain,"depthMap"), 0);
	glDrawArrays(GL_TRIANGLES,0,mdl1.getSize());
	
	//вторая
	glBindVertexArray(VAO2);
	model = glm::rotate(glm::radians(-yAngle), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::vec3(0.0f,0.0f,-20.0f))*
		glm::scale(glm::vec3(2,2,2));
	mvp=proj*view*model;
	glUniformMatrix4fv(mLoc,1,GL_FALSE,&model[0][0]);
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
	nm = glm::transpose(glm::inverse(glm::mat3x3(model)));
	glUniformMatrix3fv(nmLoc, 1, GL_FALSE, &nm[0][0]);
	glDrawArrays(GL_TRIANGLES,0,sphere.getSize());
	//третья
	glBindVertexArray(VAO2);
	model = glm::rotate(glm::radians(yAngle/2), glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::vec3(0.0f,0.0f,-40.0f))*
		glm::scale(glm::vec3(3,3,3));
	mvp=proj*view*model;
	glUniformMatrix4fv(mLoc,1,GL_FALSE,&model[0][0]);
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
	nm = glm::transpose(glm::inverse(glm::mat3x3(model)));
	glUniformMatrix3fv(nmLoc, 1, GL_FALSE, &nm[0][0]);
	glActiveTexture(GL_TEXTURE1);//
	glBindTexture(GL_TEXTURE_2D,TEX1);// 
	glUniform1i(glGetUniformLocation(ShaderMain,"normalMap"), 1);
	glDrawArrays(GL_TRIANGLES,0,sphere.getSize());
	
	//сфера в центре
	model=glm::translate(light);
	mvp=proj*view*model;
	glUseProgram(ShaderLightSource);
	glBindVertexArray(VAOLightSource);
	glUniformMatrix4fv(glGetUniformLocation(ShaderLightSource,"mvp"),1,GL_FALSE,&mvp[0][0]);
	glDrawArrays(GL_TRIANGLES,0,sphere.getSize());
	//звезды
	glBindVertexArray(starsVAO);
	mvp=proj*view;
	glUniformMatrix4fv(glGetUniformLocation(ShaderLightSource,"mvp"),1,GL_FALSE,&mvp[0][0]);
	glPointSize(2);
	glDrawArrays(GL_POINTS,0,300);
	//billboards
	glm::vec3 campos(cam.x,cam.y,cam.z);
	blst.Draw(mvp,campos,light);
	//рендер из текстуры на экран
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glViewport(0,0,Width,Height);
	glUseProgram(ShaderPost);
	glBindVertexArray(VAOPost);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,PostMap);
	glUniform1i(glGetUniformLocation(ShaderPost,"PostMap"),0);
	glUniform1i(glGetUniformLocation(ShaderPost,"Effect"),Effect);
	glUniformMatrix4fv(glGetUniformLocation(ShaderPost,"mvp"),1,GL_FALSE,&mvp[0][0]);
	glDrawArrays(GL_TRIANGLES,0,6);
	glBindVertexArray(0);
	glFlush();
	glutSwapBuffers();
	//вычисление fps
	FPS.Calculate();
}



int startx,starty;
int deltax,deltay;

void KeyDown(unsigned char key, int x, int y){
	switch (key){
		case 'w':
			cam.gf=true;
			break;
		case 's':
			cam.gb=true;
			break;
		case 'a':
			cam.gl=true;
			break;
		case 'd':
			cam.gr=true;
			break;
		case '\033':
			exit(0);
		case '1':
			rotSpeed+=1;
			break;
		case '2':
			rotSpeed-=1;
			break;
		case 'i':
			light.x+=1;
			break;
		case 'k':
			light.x-=1;
			break;
		case 'j':
			light.y+=1;
			break;
		case 'l':
			light.y-=1;
			break;
		case 'u':
			light.z+=1;
			break;
		case 'o':
			light.z-=1;
			break;
		case 'z':
			Effect++;
			if (Effect>5) Effect=0;
			break;
		case 'v':
			if (STOPBILLBOARDS) STOPBILLBOARDS=false;
			else STOPBILLBOARDS=true;
			break;
		case 'r':
			buf = ShaderCook;
			ShaderCook=ShaderMain;
			ShaderMain=buf;
			buf = VAO1Cook;
			VAO1Cook=VAO1;
			VAO1=buf;
			buf = VAO2Cook;
			VAO2Cook=VAO2;
			VAO2=buf;
			break;
	}
}
void KeyUp(unsigned char key, int x, int y){
	switch (key){
		case 'w':
			cam.gf=false;
			break;
		case 's':
			cam.gb=false;
			break;
		case 'a':
			cam.gl=false;
			break;
		case 'd':
			cam.gr=false;
			break;
		}
}
void MouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			startx=x;
			starty=y;
		}
	}
}
void MouseMove(int x, int y) {
	deltax=startx-x;
	deltay=starty-y;
	startx=x;
	starty=y;
	cam.changeAngle(deltax,deltay);
	glutPostRedisplay();
}
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_DEPTH|GLUT_DOUBLE|GLUT_MULTISAMPLE);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(500,500);
	glutCreateWindow("Compgraph");

	glewInit();

	init();
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMove);
	glutTimerFunc(20,idle,0);
	FPS.Start();
	glutMainLoop();

	return 0;
}
