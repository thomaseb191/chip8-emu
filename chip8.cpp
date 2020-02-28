#include <stdlib.h>
#include <stdio.h>
#include "chip8.h"

unsigned char chip8_fontset[80] =
{ 
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
}; //Copied directly from http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/

chip8::chip8(){

}

chip8::~chip8(){

}

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
        memory[i] = chip8_fontset[i]; //Load fontset from separate array


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

