#include <stdio.h>
#include "chip8.h"

chip8 myChip8;

int main(int argc, char **argv){

    myChip8.initialize();
    if (argc != 2) {
        printf("Args should be formatted as ./chip8-emu <path to file>\n");
        return 1;
    }
    myChip8.loadgame(argv[1]);

    while (true) {
        myChip8.emulateCycle();
    }

    return 0;
}