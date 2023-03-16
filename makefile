.PHONY: all clean count
output: main.o chip8.o render.o
	g++ main.o chip8.o render.o -o firstEmu -lglut -lGLU -lGL
main.o: main.cpp
	g++ -c main.cpp
chip8.o: chip8.cpp chip8.hpp
	g++ -c chip8.cpp
render.o: render.cpp render.hpp
	g++ -c render.cpp
clean:
	rm -rf *.o