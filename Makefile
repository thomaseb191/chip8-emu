CXX = g++
# Update these paths to match your installation
# You may also need to update the linker option rpath, which sets where to look for
# the SDL2 libraries at runtime to match your install
SDL_LIB = -lSDL2
SDL_INCLUDE = -I/usr/include/SDL2
# You may need to change -std=c++11 to -std=c++0x if your compiler is a bit older
CXXFLAGS = -c $(SDL_INCLUDE) -D_REENTRANT
LDFLAGS = $(SDL_LIB)
EXE = chip8-emu

all: $(EXE)

$(EXE): main.o chip8.o
	$(CXX) main.o chip8.o $(LDFLAGS) -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $@

chip8.o: chip8.cpp chip8.h
	$(CXX) $(CXXFLAGS) chip8.cpp -o $@

clean: 
	rm *.o && rm $(EXE)