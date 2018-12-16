#ifndef KORCHAGIN_CAMERA_CLASS
#define KORCHAGIN_CAMERA_CLASS
#include <glm/glm.hpp>
struct Camera{
	float x,y,z;
	float oxz,oyz;
	float dx,dy,dz;
	float speed;
	float sensitivity;
	bool gf,gr,gl,gb;
	Camera(){
		x=y=oxz=oyz=z=0;
		sensitivity=0.3;
		speed=0.3;
	}
	Camera(float sensitivity,float speed){
		x=y=oxz=oyz=z=0;
		this->speed=speed;
		this->sensitivity=sensitivity;
	}
	void changeAngle(float phi,float psi){
		oxz+=phi*sensitivity;
		oyz-=psi*sensitivity;
		if (oxz>=360) oxz-=360;
		else if (oxz<=0) oxz=360-oxz;
		if (oyz>85) oyz=85;
		else if (oyz<-85) oyz=-85;
	}
	void  setDirection(){
		float phi=3.14*oxz/180;
		float psi=3.14*oyz/180;
		dz=cos(phi)*cos(psi);
		dx=sin(phi)*cos(psi);
		dy=-sin(psi);
		float buf=sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2))/speed;
		dx/=buf;
		dy/=buf;
		dz/=buf;
	}
	void goForward(){
		setDirection();
		x+=dx;
		y+=dy;
		z+=dz;
	}
	void goBack(){
		setDirection();
		x-=dx;
		y-=dy;
		z-=dz;
	}
	void goRight(){
		setDirection();
		vectmul(0,1,0);
		x+=dx;
		y+=dy;
		z+=dz;
	}
	void goLeft(){
		setDirection();
		vectmul(0,-1,0);
		x+=dx;
		y+=dy;
		z+=dz;
	}
	void toDefault(){
		x=0;y=-100;z=-100;
		dx=dy=dz=oxz=oyz=0;
		oyz=-60;
	}
	void vectmul(float x,float y,float z){
		float a,b,c;
		a=dy*z-dz*y;
		b=dz*x-dx*z;
		c=dx*y-dy*x;
		dx=a;
		dy=b;
		dz=c;
	}
	void move(){
		if (gf) goForward();
		if (gb) goBack();
		if (gl) goLeft();
		if (gr) goRight();
	}
	glm::mat4x4 getView(){
		setDirection();
		return glm::lookAt(glm::vec3(x,y,z),
			glm::vec3(x+dx,y+dy,z+dz),
			glm::vec3(0,1,0));
	}
};

#endif
