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
using namespace std;
//упаковка?
struct Vertex
{
	float x, y, z;
};



GLfloat cube_vertices[] = {
	-0.5f, -0.5f, -0.5f, 1, 0, 0, 0, 0,
	-0.5f, 0.5f, -0.5f, 1, 0, 0, 1, 0,
	0.5f, 0.5f, -0.5f, 1, 0, 0, 1, 1,
	0.5f, -0.5f, -0.5f, 1, 0, 0, 0, 1,
	-0.5f, -0.5f, 0.5f, 1, 0, 0, 0, 0,
	-0.5f, 0.5f, 0.5f, 1, 0, 0, 0, 1,
	0.5f, 0.5f, 0.5f, 1, 0, 0, 1, 1,
	0.5f, -0.5f, 0.5f, 1, 0, 0, 1, 0
};

GLuint cube_indices[] = {
	0,1,2,3,
	4,5,6,7,
	0,1,5,4,
	1,2,6,5,
	2,3,7,6,
	3,0,4,7
};

Camera cam(0.1,0.5);
GLuint Width=512,Height=512;
double FPS=60;
double fps=0;
int fpsn=0;
double tEnd,tStart;

GLuint VBO1,VBO2;
GLuint VAO1,VAO2;
GLuint SDR1,SDR2;
GLuint TEX1;
//GLuint mvpLoc;

float xAngle = 0;
float yAngle = 0;

bool init() 
{
	glEnable(GL_DEPTH_TEST);
	
	SDR1=CreateShader("test.vert","test.frag");
	SDR2=CreateShader("shd.vert","shd.frag");
	
	//VBO 1
	glGenBuffers(1, &VBO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);
	//VAO 1
	glGenVertexArrays(1, &VAO1);
	glBindVertexArray(VAO1);
	int modelPos = glGetAttribLocation(SDR1, "modelPos");
	int texPos = glGetAttribLocation(SDR1,"texPos");
	int colPos = glGetAttribLocation(SDR1,"colPos");
	glVertexAttribPointer(modelPos, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(texPos, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
	glVertexAttribPointer(colPos, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(modelPos);
	glEnableVertexAttribArray(texPos);
	glEnableVertexAttribArray(colPos);
	
	//VBO 2
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);
	//VAO 2
	glGenVertexArrays(1, &VAO2);
	glBindVertexArray(VAO2);
	modelPos = glGetAttribLocation(SDR2, "modelPos");
	texPos = glGetAttribLocation(SDR2,"texPos");
	colPos = glGetAttribLocation(SDR2,"colPos");
	glVertexAttribPointer(modelPos, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
	glVertexAttribPointer(texPos, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
	glVertexAttribPointer(colPos, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
	glEnableVertexAttribArray(modelPos);
	glEnableVertexAttribArray(texPos);
	glEnableVertexAttribArray(colPos);

	//unbind VAO & VBO
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	
	//GEN textures
	Texture tex1("earth.bmp");
	glGenTextures(1, &TEX1);
	glBindTexture(GL_TEXTURE_2D, TEX1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex1.getWidth(),tex1.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, tex1.get());
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


glm::mat4x4 proj;

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
	
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	//to global
	glm::mat4x4 identity(1.0f);
	
	
	glm::mat4x4 view = cam.getView();
		
	glm::mat4x4 modl = identity *
		glm::rotate(yAngle, glm::vec3(0.0f, 1.0f, 0.0f)) *
		glm::translate(glm::vec3(0.0f,0.0f,-10.0f))*
		glm::rotate(yAngle, glm::vec3(1.0f,0.0f,1.0f));
	//вынести в инит
	
	glUseProgram(SDR1);
	glBindVertexArray(VAO1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO1);
	glBindTexture(GL_TEXTURE_2D, TEX1);
	
	GLuint modlLoc=glGetUniformLocation(SDR1,"modl");
	GLuint viewLoc=glGetUniformLocation(SDR1,"view");
	GLuint projLoc=glGetUniformLocation(SDR1,"proj");
	GLuint timeLoc=glGetUniformLocation(SDR1,"time");
	GLfloat time = omp_get_wtime();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);
	glUniformMatrix4fv(modlLoc, 1, GL_FALSE, &modl[0][0]);
	glUniform1f(timeLoc, time);
	glUniform1i(glGetUniformLocation(SDR1,"sampler"), 0);
	
	glDrawElements(GL_QUADS, sizeof(cube_indices) / sizeof(cube_indices[0]), GL_UNSIGNED_INT, cube_indices);
	
	glUseProgram(SDR2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	
	modl = identity * 
		glm::rotate(xAngle, glm::vec3(1,1,1));
	
	modlLoc=glGetUniformLocation(SDR2,"modl");
	viewLoc=glGetUniformLocation(SDR2,"view");
	projLoc=glGetUniformLocation(SDR2,"proj");
	timeLoc=glGetUniformLocation(SDR2,"time");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);
	glUniformMatrix4fv(modlLoc, 1, GL_FALSE, &modl[0][0]);
	glUniform1f(timeLoc, time);
	
	glDrawElements(GL_QUADS, sizeof(cube_indices) / sizeof(cube_indices[0]), GL_UNSIGNED_INT, cube_indices);
	
	glFlush(); 
	glutSwapBuffers();
	
	//вычисление fps
	fpsn++; //количество кадров
	tEnd=omp_get_wtime();
	FPS=tEnd-tStart;
	fps+=FPS;
	if (fps>1) {
		printf("FPS = %d\n",fpsn);
		fps=0;
		fpsn=0;
	}
	FPS=1/FPS;
	tStart=omp_get_wtime();
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
	tStart=omp_get_wtime();
	
	glutMainLoop();

	return 0;
}
