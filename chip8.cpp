#include <stdlib.h>
#include "chip8.h"

/*
 * Initialize registers and memory once
 * ie, set everything to 0
 */
void chip8::initialize() {
    pc      = 0x200; //Program counter starts at 0x200
    opcode  = 0;
    I       = 0;
    sp      = 0;

    //Clear memory
    for(int i = 0; i < 4096; i++)
        memory[i] = 0;

    //Clear registers
    for(int i = 0; i < 16; i++)
        V[i] = 0;

    //Clear stack
    for(int i = 0; i < 16; i++)
        stack[i] = 0;

    //Clear display
    for(int i = 0; i < 64*32; i++)
        gfx[i] = 0;
    
    //Load fontset
    for(int i = 0; i < 80; i++)
        memory[i] = 0; //TODO: Load fontset from separate array


}

/*
 * Load game from file
 */
void chip8::loadgame(char* filename){

}

/*
 * Fetch, decode, execute opcode
 * Update timers 
 */
void chip8::emulateCycle(){

}