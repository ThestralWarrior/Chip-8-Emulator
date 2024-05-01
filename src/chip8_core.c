#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include "chip8_core.h"

uint8_t V[NUM_REGISTERS] = {0};
uint8_t memory[MEMORY_SIZE] = {0};
uint16_t I = 0;
uint16_t PC = 0x200;
uint8_t SP = -1;
uint16_t opcode = 0;
uint16_t stack[STACK_SIZE] = {0};
uint8_t keypad[TOTAL_KEYS] = {0};
uint8_t display[TOTAL_PIXELS] = {0};
uint8_t delay = 0;
uint8_t sound = 0;
const uint8_t fontset[NUM_FONTS] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

void initializeCPU() {
    srand((unsigned int)time(NULL));
    memcpy(memory, fontset, sizeof(fontset));
}

// needs some improvement
int loadROM(char *filename) {
    FILE *fptr;
    if((fptr = fopen(filename, "rb")) == NULL) {
        printf("Error while loading rom file.\n");
        return errno;
    }
    struct stat fileinfo;
    stat(filename, &fileinfo);
    size_t bytes_read = fread(memory + 0x200, 1, sizeof(memory) - 0x200, fptr);
    fclose(fptr);
    size_t expected_size = (size_t)(fileinfo.st_size);
    if(bytes_read != expected_size)
        return -1;
    printf("Bytes read: %ld\n", bytes_read);
    return 0;
}

void executeInstruction() {
    opcode = memory[PC] << 8 | memory[PC + 1];
    uint16_t address = opcode & 0x0FFF;
    uint8_t value = opcode & 0x00FF;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    switch(opcode & 0xF000) {
        case 0x0000:
            switch(opcode & 0x00FF) {
                case 0x00E0:
                    printf("00E0: 0x%04X (Valid).\n", opcode);
                    for(int i = 0; i < 64 * 32; i++) {
                        display[i] = 0;
                    }
                    PC += 2;
                    break;
                case 0x00EE:
                    printf("00EE: 0x%04X (Valid).\n", opcode);
                    PC = stack[SP];
                    SP--;
                    PC += 2;
                    break;
                default:
                    printf("0x%04X (Invalid).\n", opcode);
                    break;
            }
            break;
        case 0x1000:
            printf("1NNN: 0x%04X (Valid).\n", opcode);
            PC = address;
            break;
        case 0x2000:
            printf("2NNN: 0x%04X (Valid).\n", opcode);
            SP++;
            stack[SP] = PC;
            PC = address;
            break;
        case 0x3000:
            printf("3XNN: 0x%04X (Valid).\n", opcode);
            if(V[x] == value) {
                PC += 2;
            }
            PC += 2;
            break;
        case 0x4000:
            printf("4XNN: 0x%04X (Valid).\n", opcode);
            if(V[x] != value) {
                PC += 2;
            }
            PC += 2;
            break;
        case 0x5000:
            printf("5XY0: 0x%04X (Valid).\n", opcode);
            if(V[x] == V[y]) {
                PC += 2;
            }
            PC += 2;
            break;
        case 0x6000:
            printf("6XNN: 0x%04X (Valid).\n", opcode);
            V[x] = value;
            PC += 2;
            break;
        case 0x7000:
            printf("7XNN: 0x%04X (Valid).\n", opcode);
            V[x] = V[x] + value;
            PC += 2;
            break;
        case 0x8000:
            switch(opcode & 0x000F) {
                case 0x0000:
                    printf("8XY0: 0x%04X (Valid).\n", opcode);
                    V[x] = V[y];
                    PC += 2;
                    break;
                case 0x0001:
                    printf("8XY1: 0x%04X (Valid).\n", opcode);
                    V[x] = V[x] | V[y];
                    PC += 2;
                    break;
                case 0x0002:
                    printf("8XY2: 0x%04X (Valid).\n", opcode);
                    V[x] = V[x] & V[y];
                    PC += 2;
                    break;
                case 0x0003:
                    printf("8XY3: 0x%04X (Valid).\n", opcode);
                    V[x] = V[x] ^ V[y];
                    PC += 2;
                    break;
                case 0x0004:
                    printf("8XY4: 0x%04X (Valid).\n", opcode);
                    int sum = V[x] + V[y];
                    V[x] = sum % 256;
                    if(sum > 255) V[0xF] = 1;
                    else V[0xF] = 0;
                    PC += 2;
                    break;
                case 0x0005:
                    printf("8XY5: 0x%04X (Valid).\n", opcode); 
                    V[x] -= V[y];
                    if(V[x] > V[y]) V[0xF] = 1;
                    else V[0xF] = 0;
                    PC += 2;
                    break;
                case 0x0006:
                    printf("8XY6: 0x%04X (Valid).\n", opcode);
                    
                    V[x] = V[x] >> 1;
                    V[0xF] = V[x] & 0x1;
                    PC += 2;
                    break;
                case 0x0007:
                    printf("8XY7: 0x%04X (Valid).\n", opcode);
                    V[x] = V[y] - V[x];
                    if(V[y] > V[x]) V[0xF] = 1;
                    else V[0xF] = 0;
                    PC += 2;
                    break;
                case 0x000E:
                    printf("8XYE: 0x%04X (Valid).\n", opcode);
                    
                    V[x] = V[x] << 1;
                    V[0xF] = (V[x] & 0x80) >> 7;
                    PC += 2;
                    break;
                default:
                    printf("0x%04X (Invalid).\n", opcode);
                    break;
            }
            break;
        case 0x9000:
            printf("9XY0: 0x%04X (Valid).\n", opcode);
            if(V[x] != V[y]) {
                PC += 2;
            }
            PC += 2;
            break;
        case 0xA000:
            printf("ANNN: 0x%04X (Valid).\n", opcode);
            I = address;
            PC += 2;
            break;
        case 0xB000:
            printf("BNNN: 0x%04X (Valid).\n", opcode);
            PC = address + V[0];
            break;
        case 0xC000:
            printf("CXNN: 0x%04X (Valid).\n", opcode);
            int random = rand() % 256;
            V[x] = random & value;
            PC += 2;
            break;
        case 0xD000:
            printf("DXYN: 0x%04X (Valid).\n", opcode);
            uint16_t height = opcode & 0x000F;
            uint16_t pixel;

            V[0xF] = 0;

            for (int row = 0; row < height; row++) {
                pixel = memory[I + row];
                for (int column = 0; column < 8; column++) {
                    if ((pixel & (0x80 >> column)) != 0) {
                        if (display[(V[x] + column + ((V[y] + row) * 64))] ==
                            1) {
                            V[0xF] = 1;
                        }
                        display[V[x] + column + ((V[y] + row) * 64)] ^= 1;
                    }
                }
            }

            PC += 2;
            break;
        case 0xE000:
            switch(opcode & 0x00FF) {
                case 0x009E:
                    printf("EX9E: 0x%04X (Valid).\n", opcode);
                    if(keypad[V[x]]) {
                        PC += 2;
                    }
                    PC += 2;
                    break;
                case 0x00A1:
                    printf("EXA1: 0x%04X (Valid).\n", opcode);
                    if(!keypad[V[x]]) {
                        PC += 2;
                    }
                    PC += 2;
                    break;
                default:
                    printf("0x%04X (Invalid).\n", opcode);
                    break;
            }
            break;
        case 0xF000:
            switch(opcode & 0x00FF) {
                case 0x0007:
                    printf("FX07: 0x%04X (Valid).\n", opcode);
                    V[x] = delay;
                    PC += 2;
                    break;
                case 0x000A:
                    printf("FX0A: 0x%04X (Valid).\n", opcode);
                    for(int i = 0; i < 16; i++) {
                        if(keypad[i]) {
                            V[x] = i;
                            PC += 2;
                            break;
                        }
                    }
                    break;
                case 0x0015:
                    printf("FX15: 0x%04X (Valid).\n", opcode);
                    delay = V[x];
                    PC += 2;
                    break;
                case 0x0018:
                    printf("FX18: 0x%04X (Valid).\n", opcode);
                    sound = V[x];
                    PC += 2;
                    break;
                case 0x001E:
                    printf("FX1E: 0x%04X (Valid).\n", opcode);
                    I = I + V[x];
                    PC += 2;
                    break;
                case 0x0029:
                    printf("FX29: 0x%04X (Valid).\n", opcode);
                    I = V[x] * 5;
                    PC += 2;
                    break;
                case 0x0033:
                    printf("FX33: 0x%04X (Valid).\n", opcode);
                    int ones = V[x] % 10, tens = (V[x] / 10) % 10, hundreds = (V[x] / 100) % 10;
                    memory[I] = hundreds;
                    memory[I + 1] = tens;
                    memory[I + 2] = ones;
                    PC += 2;
                    break;
                case 0x0055:
                    printf("FX55: 0x%04X (Valid).\n", opcode);
                    for(int i = 0; i <= x; i++) {
                        memory[I + i] = V[i];
                    }
                    PC += 2;
                    break;
                case 0x0065:
                    printf("FX65: 0x%04X (Valid).\n", opcode);
                    for(int i = 0; i <= x; i++) {
                        V[i] = memory[I + i];
                    }
                    PC += 2;
                    break;
                default:
                    printf("0x%04X (Invalid).\n", opcode);
                    break;
            }
            break;
        default:
            printf("0x%04X (Invalid).\n", opcode);
            break;
        if(delay > 0)
            delay -= 1;
        if(sound > 0) {
            sound -= 1;
            printf("beep");
        }
    }
}

void printInstructions(char *filename) {
    FILE *fptr = NULL;
    if((fptr = fopen(filename, "rb")) == NULL) {
        printf("Couldn't load rom file.\n");
        return;
    }
    uint16_t op;
    printf("Instruction in %s: ", filename);
    while(fread(&op, sizeof(uint16_t), 1, fptr) == 1) {
        op = ((op & 0xFF) << 8) | ((op & 0xFF00) >> 8);
        printf("0x%04X ", op);
    }
    printf("\n");
}