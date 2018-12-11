#ifndef KORCHAGIN_FPS_COUNTER
#define KORCHAGIN_FPS_COUNTER
#include <stdio.h>
#include <omp.h>
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
#endif
