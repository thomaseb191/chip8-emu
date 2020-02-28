chip8: main.o chip8.o
	g++ -o chip8-emu main.o chip8.o

main.o: main.cpp
	g++ -c main.cpp

chip8.o: chip8.cpp chip8.h
	g++ -c chip8.cpp