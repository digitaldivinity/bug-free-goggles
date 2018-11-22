other:
	sudo apt-get install libglew-dev libglm-dev freeglut3-dev
test: test.cpp test.frag test.vert shdrprg.h shaderloader.h camera.h Texture.h
	g++ test.cpp -o test -fopenmp -lm -lSOIL -lGLEW -lGL -lGLU -lglut
run: test
	./test
