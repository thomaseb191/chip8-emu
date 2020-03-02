#include <stdio.h>
#include <SDL2/SDL.h>
#include "chip8.h"

chip8 myChip8;

SDL_Event Events;
SDL_Window* win;
SDL_Renderer* ren;
#define PIXSIZE 10
#define WIDTH 64
#define HEIGHT 32

Uint32 waittime = 1000.0f/60.0f;
Uint32 framestarttime = 0;
Sint32 delaytime;
bool isRunning = 1;

int initgraphics();
void draw();

int main(int argc, char **argv){

    myChip8.initialize();
    if (argc != 2) {
        printf("Args should be formatted as ./chip8-emu <path to file>\n");
        return 1;
    }
    myChip8.loadgame(argv[1]);

    if (initgraphics()) return 1;

    while (isRunning) {
        while (SDL_PollEvent(&Events))
        {
            if (Events.type == SDL_QUIT) isRunning = false;
        }

        myChip8.emulateCycle();

        if (myChip8.drawflag) draw();

        //Run at 60 Hz
        delaytime = waittime - (SDL_GetTicks() - framestarttime);
        if (delaytime > 0) SDL_Delay((Uint32)delaytime);
        framestarttime = SDL_GetTicks();
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    

    return 0;
}


int initgraphics(){
    //Init SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    //Init window
    win = SDL_CreateWindow("Chip-8", 100, 100, WIDTH * PIXSIZE, HEIGHT * PIXSIZE, 0);
    if (win == NULL){
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    //Init renderer
    ren = SDL_CreateRenderer(win, -1, 0);
    if (ren == NULL){
        SDL_DestroyWindow(win);
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_Quit();
    }

    return 0;
}

void draw(){
    if(SDL_SetRenderDrawColor(ren, 0, 0, 0, 255)) fprintf(stderr, "SDL_SetRenderDrawColor Error: %s\n", SDL_GetError());
    if(SDL_RenderClear(ren)) fprintf(stderr, "SDL_RenderClear Error: %s\n", SDL_GetError());
    SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
    for (int x = 0; x < 64; x++){
        for (int y = 0; y < 32; y++){
            if(myChip8.gfx[x + y*64] == 0) continue;
            SDL_Rect pix;
            pix.x = x*PIXSIZE;
            pix.y = y*PIXSIZE;
            pix.h = PIXSIZE;
            pix.w = PIXSIZE;
            SDL_RenderFillRect(ren, &pix);
        }
    }
    myChip8.drawflag = false;
    SDL_RenderPresent(ren);
}