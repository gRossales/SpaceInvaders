all:
	g++ -std=c++11 -o Space Space.cpp -lglut -lGLU -lGL -lm
run:
	./Space