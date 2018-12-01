other:
	sudo apt-get install libglew-dev libglm-dev freeglut3-dev
	sudo apt-get install libassimp-dev libsoil-dev
test: test.cpp shaders/main.vert shaders/main.frag shaders/light.vert shaders/light.frag shdrprg.h shaderloader.h camera.h Texture.h objloader.h
	g++ test.cpp -o test -fopenmp -lm -lSOIL -lGLEW -lGL -lGLU -lglut
run: test
	./test
