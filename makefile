other:
	sudo apt-get install libglew-dev libglm-dev freeglut3-dev libsoil-dev
test: test.cpp shaders/main.vert shaders/main.frag shaders/light.vert shaders/light.frag Shader.h Camera.h Texture.h Model.h 
	g++ test.cpp -o test -fopenmp -lm -lSOIL -lGLEW -lGL -lGLU -lglut
run: test
	./test
crun:
	g++ test.cpp -o test -fopenmp -lm -lSOIL -lGLEW -lGL -lGLU -lglut
	./test
