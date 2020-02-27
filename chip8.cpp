#include <stdlib.h>
#include <stdio.h>
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

    printf("Loading: %s\n", filename);

    //Check file exists
    FILE * rom = fopen(filename, "rb");
    if (rom == NULL){
        fprintf(stderr, "File error");
        return;
    }

    //Check file size
    fseek(rom, 0, SEEK_END);
    long romsize = ftell(rom);
    rewind(rom);
    printf("Filesize: %d\n", (int)romsize);

    //Allocate memory to hold file
    char* buffer = (char*)malloc(sizeof(char) * romsize);
    if (buffer == NULL){
        fprintf(stderr, "Memory error");
        return;
    }

    //Copy file into buffer
    size_t result = fread(buffer, 1, romsize, rom);
    if (result != romsize) {
        fprintf(stderr, "Reading error");
        return;
    }

    //Copy buffer into chip8 mem
    if((4096-512) > romsize){
        for (int i = 0; i < romsize; i++)
            memory[i + 0x200] = buffer[i]; //Offset for program
    } else {
        printf("Error: ROM too big for memory");
    }

    fclose(rom);
    free(buffer);

    return;
}

/*
 * Fetch, decode, execute opcode
 * Update timers 
 */
void chip8::emulateCycle(){

}