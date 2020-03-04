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

Uint32 waittime = 10;
Uint32 framestarttime = 0;
Sint32 delaytime;
bool isRunning = 1;

int initgraphics();
void draw();
void keyboardChange(SDL_KeyboardEvent* , bool);

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
            switch(Events.type){
                case SDL_QUIT:
                    isRunning = false;
                    break;
                case SDL_KEYDOWN:
                    keyboardChange(&Events.key, true);
                    break;
                case SDL_KEYUP:
                    keyboardChange(&Events.key, false);
                    break;
                default:
                    break;
            }
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

void keyboardChange(SDL_KeyboardEvent* key, bool pressedDown){
    if (pressedDown) printf("Key: %c pressed\n", key->keysym.sym);
    else printf("Key: %c released\n", key->keysym.sym);
    switch (key->keysym.sym){
        case '1':
            myChip8.key[0x0] = pressedDown;
            break;
        case '2':
            myChip8.key[0x1] = pressedDown;
            break;
        case '3':
            myChip8.key[0x2] = pressedDown;
            break;
        case '4':
            myChip8.key[0x3] = pressedDown;
            break;
        case 'q':
            myChip8.key[0x4] = pressedDown;
            break;
        case 'w':
            myChip8.key[0x5] = pressedDown;
            break;
        case 'e':
            myChip8.key[0x6] = pressedDown;
            break;
        case 'r':
            myChip8.key[0x7] = pressedDown;
            break;
        case 'a':
            myChip8.key[0x8] = pressedDown;
            break;
        case 's':
            myChip8.key[0x9] = pressedDown;
            break;
        case 'd':
            myChip8.key[0xa] = pressedDown;
            break;
        case 'f':
            myChip8.key[0xb] = pressedDown;
            break;
        case 'z':
            myChip8.key[0xc] = pressedDown;
            break;
        case 'x':
            myChip8.key[0xd] = pressedDown;
            break;
        case 'c':
            myChip8.key[0xe] = pressedDown;
            break;
        case 'v':
            myChip8.key[0xf] = pressedDown;
            break;
        default:
            printf("No key binding for %c\n", key->keysym.sym);
            break;
    }
}