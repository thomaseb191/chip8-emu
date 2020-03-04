#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "chip8.h"

#define WIDTH 64
#define HEIGHT 32

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
    drawflag = 0;   //

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

    //Reset keys
    for (int i = 0; i < 16; i++)
        key[i] = 0;

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

    srand(time(NULL)); //Initialize randome seed

    fclose(rom);
    free(buffer);

    return;
}

/*
 * Fetch, decode, execute opcode
 * Update timers 
 */
void chip8::emulateCycle(){
    //Fetch
    opcode = memory[pc] << 8 | memory[pc + 1];
    //printf("pc: %.4X | opcode: 0x%.4X\n", pc, opcode);

    switch (opcode & 0xF000) {
    case 0x0000:
        switch (opcode & 0x000F) {
        case 0x0000: //0x00E0: Clears the screen
            for (int i = 0; i < 64*32; i++)
                gfx[i] = 0;
            pc += 2;
            break;

        case 0x000E: //0x00EE: Return from a subroutine
            sp--;
            pc = stack[sp];
            pc += 2;
            break;
        
        default:
            printf("0x%.4X invalid opcode\n", opcode);
            break;
        }
        break;
    
    case 0x1000: //0x1nnn: Jump to addr
        pc = opcode & 0x0FFF;
        break;
    
    case 0x2000: //0x2nnn: Call subroutine at addr
        stack[sp] = pc;
        sp++;
        pc = opcode & 0x0FFF;
        break;

    case 0x3000: //0x3xkk: Skip next instruction if Vx=kk
        if (V[(opcode >> 8) & 0x000F] == (opcode & 0x00FF)) pc += 2;
        pc += 2;
        break;

    case 0x4000: //0x4xkk: Skip next instruction if Vx!=kk
        if (V[(opcode >> 8) & 0x000F] != (opcode & 0x00FF)) pc += 2;
        pc += 2;
        break;

    case 0x5000: //0x5xy0: Skip next instruction if Vx = Vy
        if (V[(opcode >> 8) & 0x000F] == V[(opcode >> 4) & 0x000F]) pc += 2;
        pc += 2;
        break;
    
    case 0x6000: //0x6xkk: Set Vx = kk
        V[(opcode >> 8) & 0x000F] = (opcode & 0x00FF);
        pc += 2;
        break;

    case 0x7000: //0x7xkk: Set Vx = Vx + kk
        V[(opcode >> 8) & 0x000F] = V[(opcode >> 8) & 0x000F] + opcode & 0x00FF;
        pc += 2;
        break;

    case 0x8000: //Enter math mode
    {
        unsigned short x = (opcode >> 8) & 0x000F;
        unsigned short y = (opcode >> 4) & 0x000F;
        switch (opcode & 0x000F){
        case 0x0000: //0x8xy0: Set Vx = Vy
            V[x] = V[y];
            break;
        
        case 0x0001: //0x8xy1: Set Vx = Vx OR Vy
            V[x] = V[x] | V[y];
            break;
        
        case 0x0002: //0x8xy2: Set Vx = Vx AND Vy
            V[x] = V[x] & V[y];
            break;

        case 0x0003: //0x8xy3: Set Vx = Vx XOR Vy
            V[x] = V[x] ^ V[y];
            break;

        case 0x0004: //0x8xy4: Set Vx = Vx + Vy
        {
            unsigned short r = V[x] + V[y];
            if (r > 0xFF) V[0xF] = 1;
            else V[0xF] = 0;
            V[x] = r & 0xFF;
            break;
        }

        case 0x0005: //0x8xy5: Set Vx = Vx - Vy
            if (V[x] > V[y]) V[0xF] = 1;
            else V[0xF] = 0;
            V[x] = V[x] - V[y];
            break;    

        case 0x0006: //0x8xy6: Set Vx = Vx SHR 1
            V[0xF] = V[x] & 0x1;
            V[x] = V[x] >> 1;
            break;

        case 0x0007: //0x8xy7: Set Vx = Vy - Vx
            if (V[y] > V[x]) V[0xF] = 1;
            else V[0xF] = 0;
            V[x] = V[y] - V[x];
            break;
        
        case 0x000E: //0x8xyE: Set Vx = Vx SHL 1
            V[0xF] = V[x] & 0x8;
            V[x] = V[x] << 1;
            break;

        default:
            printf("0x%.4X invalid opcode\n", opcode);
            break;
        }
        pc += 2;
        break;
    }

    case 0x9000: //0x9xy0: Skip next instruction if Vx != Vy
        if (V[(opcode >> 8) & 0x000F] != V[(opcode >> 4) & 0x000F]) pc += 2;
        pc += 2;
        break;

    case 0xA000: //0xAnnn: Set I = nnn
        I = (opcode & 0x0FFF);
        pc += 2;
        break;
    
    case 0xB000: //0xBnnn: Jump to location nnn + V0
        pc = V[0] + (opcode & 0x0FFF);
        break;

    case 0xC000: //0xCxkk: Set Vx = rand AND kk
        V[(opcode >> 8) & 0x000F] = (rand() % 256) & opcode;
        pc += 2;
        break;

    case 0xD000: //0xDxyn: Display n-byte sprite
    {
        unsigned short x = V[(opcode >> 8) & 0x000F];
        unsigned short y = V[(opcode >> 4) & 0x000F];
        unsigned short height = opcode & 0x000F;
        unsigned short pixels;

        V[0xF] = 0; //Reset Vf

        for (int yoffset = 0; yoffset < height; yoffset++){
            pixels = memory[I + yoffset];
            for (int xoffset = 0; xoffset < 8; xoffset++){
                if ((pixels & (0x80 >> xoffset)) != 0){
                    unsigned short truex = (x + xoffset) % WIDTH;
                    unsigned short truey = (y + yoffset) % HEIGHT;
                    if (gfx[truex + truey*WIDTH] == 1) V[0xF] = 1;
                    gfx[truex + truey*WIDTH] ^= 1;
                    drawflag = true;
                }
            }

        }

        pc+=2;
        break;
    }

    case 0xE000:
        switch (opcode & 0x00FF){
        case 0x009E: //0xEx9E Skip next instruction if key Vx pressed
        {
            unsigned short k = V[(opcode >> 8) & 0x000F];
            if (key[k]) pc += 2;
            break;
        }
        case 0x00A1: //0xExA1 Skip next instruction if key Vx not pressed
        {
            unsigned short k = V[(opcode >> 8) & 0x000F];
            if (key[k] != true) pc += 2;
            break;
        }
        default:
            break;
        }
        pc += 2;
        break;

    case 0xF000:
        switch (opcode & 0x00FF){
        case 0x0007: //0xFx07: Set Vx to delay timer
            V[(opcode >> 8) & 0x000F] = delay_timer;
            pc += 2;
            break;

        case 0x000A: //0xFx0A: Wait for a key press, store the value in Vx
        {
            bool keyPressed = false;

            for (int i = 0; i < 16; i++){
                if (key[i]) {
                    keyPressed = true;
                    V[(opcode >> 8) & 0x000F] = i;
                }
            }
            if (keyPressed) pc += 2;
            break;

        }

        case 0x0015: //0xFx15: Set delay timer to Vx
            delay_timer = V[(opcode >> 8) & 0x000F];
            pc += 2;
            break;

        case 0x0018: //0xFx18: Set sound timer to Vx
            sound_timer = V[(opcode >> 8) & 0x000F];
            pc += 2;
            break;

        case 0x001E: //0xFx1E: Set I = I + Vx
            I = I + V[(opcode >> 8) & 0x000F];
            pc += 2;
            break;

        case 0x0029: //0xFx29: Set I = location of sprite Vx
            I = V[(opcode >> 8) & 0x000F] * 5;
            pc += 2;
            break;

        case 0x0033:
        {
            unsigned char value = V[(opcode >> 8) & 0x000F];
            //printf("I = %d\n", I);
            memory[I] = value / 100;
            memory[I + 1] = (value / 10) % 10;
            memory[I + 2] = value % 10;
            pc += 2;
            break;
        }

        case 0x0055:
            for (int x = 0; x <= ((opcode >> 8) & 0x000F); x++){
                 memory[I + x] = V[x];
            }
            pc += 2;
            break;

        case 0x0065:
            for (int x = 0; x <= ((opcode >> 8) & 0x000F); x++){
                //printf("I = %d\n", I + x);
                V[x] = memory[I + x];
            }
            pc += 2;
            break;

        default:
            printf("0x%.4X invalid opcode\n", opcode);
            pc+=2;
            break;
        }
        break;

    default:
        printf("0x%.4X invalid opcode\n", opcode);
        pc+=2;
        break;
    }

    if (delay_timer) delay_timer--;
    if (sound_timer) sound_timer--;
}

