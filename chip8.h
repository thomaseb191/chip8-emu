class chip8 {
    public:
        chip8();
        ~chip8();

        void initialize();
        void loadgame();
        void emulateCycle();

    private:
        unsigned short opcode; //Represents one of 35 opcodes

        unsigned char memory[4096]; //4K worth of memory
        unsigned char V[16]; //15 8-bit general use registers, 1 carry flag register

        unsigned short I; //Index register
        unsigned short pc; //Program counter

        unsigned char gfx[64 * 32]; //Screen is 64x32 pixels

        unsigned char delay_timer;
        unsigned char sound_timer;
};