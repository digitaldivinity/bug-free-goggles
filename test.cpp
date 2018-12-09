#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>

//эксперементальное расширение glm, возможность засылать в транслейты vec
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <numeric>
#include <omp.h>
#include <SOIL/SOIL.h>

#include "shdrprg.h"
#include "camera.h"
#include "Texture.h"
#include "objloader.h"
using namespace std;

GLfloat cube_vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0, 0,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1, 0,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1, 1,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1, 1,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0, 1,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0, 0,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0, 0,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1, 0,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1, 1,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1, 1,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0, 1,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0, 0,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0, 0,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1, 0,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1, 1,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1, 1,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0, 1,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0, 0,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0, 0,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1, 0,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1, 1,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1, 1,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0, 1,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0, 0,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0, 0,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1, 0,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1, 1,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1, 1,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0, 1,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0, 0,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0, 0,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1, 0,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1, 1,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1, 1,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0, 1,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0, 0
};
GLfloat cube_vertices2[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0, 0,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1, 0,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1, 1,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1, 1,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0, 1,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0, 0,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0, 0,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1, 0,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1, 1,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  1, 1,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0, 1,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,  0, 0,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0, 0,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1, 0,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1, 1,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1, 1,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0, 1,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0, 0
};
GLuint cube_indices2[] = {
	0,1,2,
	3,4,5,
	6,7,8,
	9,10,11,
	12,13,14,
	15,16,17

};
GLfloat stars[900];
GLuint cube_indices[] = {
	0,1,2,
	3,4,5,
	6,7,8,
	9,10,11,
	12,13,14,
	15,16,17,
	18,19,20,
	21,22,23,
	24,25,26,
	27,28,29,
	30,31,32,
	33,34,35
};


Model sortir("models/sortir.obj");
Model sphere("models/sphere.obj");

Camera cam(0.1,0.5);
GLuint Width=512,Height=512;

GLuint VBO1,VBO2,lightSrcVAO;
GLuint VAO1,VAO2;
GLuint SDR1,SDR2,DepthSDR;
GLuint TEX1;
GLuint starsVAO,starsVBO;
GLuint depthMapFBO;
GLuint depthMap;
GLuint depthVAO;
GLuint SHADOW_WIDTH=1024,SHADOW_HEIGHT=1024;
//GLuint mvpLoc;

float xAngle = 0;
float yAngle = 0;

class FPScounter{
	int frames;
	double FPS;
	double time;
	double tStart;
	double tEnd;
	public:
	FPScounter(){
		frames=0;
		time=0;
		FPS=60;
	}
	void Calculate(){
		tEnd=omp_get_wtime();
		frames++;
		time+=tEnd-tStart;
		if (time>1) {
			printf("FPS = %d\n",frames);
			time=0;
			frames=0;
		}
		tStart=omp_get_wtime();
	}
	void Start(){
		tStart=omp_get_wtime();
	}
};

FPScounter FPS;

glm::mat4x4 proj;
glm::mat4x4 model;
glm::mat4x4 view;
glm::mat4x4 identity(1);

glm::mat4x4 mvp;
glm::mat4x4 mv;
glm::mat3x3 nm;

bool init()
{
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
	GLfloat * ver = sortir.getVertices();
	for (int i=0;i<1000;i+=8){
		for (int j=0;j<8;j++)
		printf("%f\t",ver[i+j]);
		printf("\n");
	}
	glEnable(GL_DEPTH_TEST);

	SDR1=CreateShader("shaders/main.vert","shaders/main.frag");
	SDR2=CreateShader("shaders/light.vert","shaders/light.frag");
	DepthSDR=CreateShader("shaders/depth.vert","shaders/depth.frag");
	printf("%d %d\n",sizeof(cube_vertices),sizeof(sortir.getVertices()));
	//VBO 1
	glGenBuffers(1, &VBO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sphere.getSize()*8*sizeof(GLfloat), sphere.getVertices(), GL_STATIC_DRAW);
	//VAO 1
	glGenVertexArrays(1, &VAO1);
	glBindVertexArray(VAO1);
	glGenBuffers(1,&VBO2);
	glBindBuffer(GL_ARRAY_BUFFER,VBO2);
	glBufferData(GL_ARRAY_BUFFER,sortir.getSize()*8*sizeof(GLfloat),sortir.getVertices(),GL_STATIC_DRAW);
	int modelPos = glGetAttribLocation(SDR1, "position");
	int texPos = glGetAttribLocation(SDR1,"texture");
	int norPos = glGetAttribLocation(SDR1,"normal");
	glVertexAttribPointer(modelPos, 3, GL_FLOAT, GL_FALSE,8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(norPos,3,GL_FLOAT,GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glVertexAttribPointer(texPos, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
	
	glEnableVertexAttribArray(modelPos);
	glEnableVertexAttribArray(texPos);
	glEnableVertexAttribArray(norPos);
	//lightsrcVAO
	glGenVertexArrays(1,&lightSrcVAO);
	glBindVertexArray(lightSrcVAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO1);
	glVertexAttribPointer(glGetAttribLocation(SDR2,"position"),
		3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glEnableVertexAttribArray(0);
	//depthVAO
	glGenVertexArrays(1,&depthVAO);
	glBindVertexArray(depthVAO);
	glBindBuffer(GL_ARRAY_BUFFER,VBO2);
	glVertexAttribPointer(glGetAttribLocation(DepthSDR,"position"),
		3,GL_FLOAT,GL_FALSE,8*sizeof(GLfloat),0);
	glEnableVertexAttribArray(0);
	//starsVAO
	glGenVertexArrays(1,&starsVAO);
	glBindVertexArray(starsVAO);
	glGenBuffers(1,&starsVBO);
	glBindBuffer(GL_ARRAY_BUFFER,starsVBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(stars),stars,GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(SDR2,"position"),
		3, GL_FLOAT, GL_FALSE, 0 , 0);
	glEnableVertexAttribArray(0);
	
	glBindVertexArray(0);
	
	//unbind VAO & VBO
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);

	//GEN textures
	Texture tex1("textures/melon.bmp");
	glGenTextures(1, &TEX1);
	glBindTexture(GL_TEXTURE_2D, TEX1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex1.getWidth(),tex1.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, tex1.get());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	

	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	
	//буффер для карты глубины
	glGenFramebuffers(1, &depthMapFBO);
	glBindTexture(GL_TEXTURE_2D,depthMap);
	glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT,SHADOW_WIDTH,SHADOW_HEIGHT,0,GL_DEPTH_COMPONENT,GL_FLOAT,NULL);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glBindFramebuffer(GL_FRAMEBUFFER,depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthMap,0);
	glDrawBuffer(GL_NONE);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	
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
	xAngle+=rotSpeed;
	yAngle+=rotSpeed;
	if (xAngle > 360) xAngle-=360;
	if (yAngle > 360) yAngle-=360;
	glutPostRedisplay();
	glutTimerFunc(20,idle,0);
}


void display(void)
{
	//не работает
	glViewport(0,0,SHADOW_WIDTH,SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER,depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	
	proj=glm::perspective(45.0f,(float)SHADOW_WIDTH/SHADOW_HEIGHT,1.0f,500.0f);
	view=glm::lookAt(glm::vec3(0,0,0),glm::vec3(1,0,0),glm::vec3(0,1,0));
	mvp=proj*view;
	glUseProgram(DepthSDR);
	glBindVertexArray(depthVAO);
	glUniformMatrix4fv(glGetUniformLocation(DepthSDR,"lightSpaceMatrix"),1,GL_FALSE,&mvp[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(DepthSDR,"model"),1,GL_FALSE,&identity[0][0]);
	
	glDrawArrays(GL_TRIANGLES,0,sortir.getSize());
	
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	
	glViewport(0,0,Width,Height);
	proj=glm::perspective(45.0f,(float)Width/Height,1.0f,500.0f);
	GLuint mvpLoc,mvLoc,nmLoc;
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	view=cam.getView();
	
	mvp = proj * view;
	glUseProgram(SDR2);
	glBindVertexArray(lightSrcVAO);
	mvpLoc=glGetUniformLocation(SDR2,"mvp");
	glUniformMatrix4fv(mvpLoc,1,GL_FALSE,&mvp[0][0]);
	glDrawArrays(GL_TRIANGLES,0,sphere.getSize());
	//glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(cube_indices[0]), GL_UNSIGNED_INT, cube_indices);
	
	glBindVertexArray(starsVAO);
	glUniformMatrix4fv(mvpLoc,1,GL_FALSE,&mvp[0][0]);
	glDrawArrays(GL_POINTS,0,300);
	
	
	model = glm::rotate(yAngle, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::vec3(0.0f,0.0f,-10.0f));
	//	glm::rotate(yAngle, glm::vec3(1.0f,0.0f,1.0f));
	mv=view*model;
	nm = glm::transpose(glm::inverse(glm::mat3x3(model)));
	mvp = proj * mv;
	
	
	glUseProgram(SDR1);
	glBindVertexArray(VAO1);
	//glBindBuffer(GL_ARRAY_BUFFER,VBO2);
	glBindTexture(GL_TEXTURE_2D, TEX1);
	mvpLoc=glGetUniformLocation(SDR1,"mvp");
	mvLoc=glGetUniformLocation(SDR1,"mv");
	nmLoc=glGetUniformLocation(SDR1,"nm");
	GLuint mLoc=glGetUniformLocation(SDR1,"m");
	GLuint camposLoc=glGetUniformLocation(SDR1,"camPosition");
	GLuint depthMapLoc=glGetUniformLocation(SDR1,"depthMap");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix3fv(nmLoc, 1, GL_FALSE, &nm[0][0]);
	glUniformMatrix4fv(mLoc,1,GL_FALSE,&model[0][0]);
	glUniform3f(camposLoc,cam.x,cam.y,cam.z);
	//glUniform1i(depthMapLoc,0);
	glBindTexture(GL_TEXTURE_2D,depthMap);
	glUniform1i(glGetUniformLocation(SDR1,"depthMap"), 0);
	//glDrawElements(GL_TRIANGLES, sizeof(cube_indices) / sizeof(cube_indices[0]), GL_UNSIGNED_INT, cube_indices);
	glDrawArrays(GL_TRIANGLES,0,sortir.getSize());
	
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
		case 'z':
			cam.toDefault();
			break;
		case '\033':
			exit(0);
		case '1':
			rotSpeed+=0.05;
			break;
		case '2':
			rotSpeed-=0.05;
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
		case 'k':
			printf("Exit\n");
			glutLeaveMainLoop();
			printf("Exit\n");
		}
}


void MouseButton(int button, int state, int x, int y) {
	// только при начале движения, если нажата левая кнопка
	if (button == GLUT_LEFT_BUTTON) {
		// когда кнопка отпущена
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
	glutCreateWindow("Animated cube");

	glewInit();

	init();
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(KeyDown);
	glutKeyboardUpFunc(KeyUp);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMove);
	//idle?
	glutTimerFunc(20,idle,0);
	FPS.Start();
	glutMainLoop();

	return 0;
}
