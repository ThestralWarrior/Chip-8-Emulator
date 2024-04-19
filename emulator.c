#include<stdio.h>
#include<stdint.h>
#include<sys/stat.h>
#include<errno.h>
#include<time.h>

#define MEMORY_SIZE 4096

extern int errno;

uint8_t V[16] = {0};
uint8_t memory[MEMORY_SIZE] = {0};
uint16_t I = 0;
uint16_t PC = 0x200;
uint8_t SP = -1;
uint16_t opcode = 0;
uint16_t stack[16] = {0};
uint8_t keypad[16] = {0};
uint8_t display[64 * 32] = {0};
uint8_t delay = 0;
uint8_t sound = 0;
const uint8_t fontset[80] = {
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
uint8_t drawFlag = 0;
uint8_t soundFlag = 0;

void init_cpu() {
    memcpy(memory, fontset, sizeof(fontset));
}

int load_rom(char *filename) {
    FILE *fptr;
    if((fptr = fopen(filename, "rb")) == NULL) {
        printf("Error while loading rom file");
        return errno;
    }
    struct stat fileinfo;
    stat(filename, fptr);
    size_t bytes_read = fread(memory + 0x200, 1, sizeof(memory) - 0x200, fptr);
    fclose(fptr);
    if(bytes_read = fileinfo.st_size)
        return -1;
    return 0;
}

void emulate_cycle() {
    drawFlag = 0;
    soundFlag = 0;
    opcode = memory[PC] << 8 | memory[PC + 1];
    uint16_t address = 0;
    uint8_t value = 0;
    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    switch(opcode & 0xF000) {
        case 0x0000:
            switch(opcode & 0x00FF) {
                case 0x00E0:
                    int i;
                    for(i = 0; i < 64 * 32; i++) {
                        display[i] = 0;
                    }
                    PC += 2;
                    break;
                case 0x00EE:
                    PC = stack[SP--];
                    PC += 2;
                    break;
                default:
                    break;
            }
        case 0x1000:
            address = opcode & 0x0FFF;
            PC = address;
            break;
        case 0x2000:
            address = opcode & 0x0FFF;
            stack[++SP] = PC;
            PC = address;
            break;
        case 0x3000:
            value = opcode & 0x00FF; 
            if(V[x] == value) {
                PC += 2;
            }
            break;
        case 0x4000:
            value = opcode & 0x00FF;
            if(V[x] != value) {
                PC += 2;
            }
            break;
        case 0x5000:
            if(V[x] == V[y]) {
                PC += 2;
            }
            break;
        case 0x6000:
            value = opcode & 0x00FF;
            V[x] = value;
            break;
        case 0x7000:
            value = opcode & 0x00FF;
            V[x] = V[x] + value;
            break;
        case 0x8000:
            switch(opcode & 0x000F) {
                case 0x0000:
                case 0x0001:
                case 0x0002:
                case 0x0003:
                case 0x0004:
                case 0x0005:
                case 0x0006:
                case 0x0007:
                case 0x000E:
                default:
                    break;
            }
        case 0x9000:
            if(V[x] != V[y]) {
                PC += 2;
            }
            break;
        case 0xA000:
            address = opcode & 0x0FFF;
            I = address;
            break;
        case 0xB000:
            address = opcode & 0x0FFF;
            PC = address + V[0];
            break;
        case 0xC000:
            value = opcode & 0x00FF;
            srand(time(NULL));
            int random = rand() % 256;
            V[x] = random & value;
            break;
        case 0xD000:
            int height = opcode & 0x000F;
            int row, column;
            for(row = 0; row < height; row++) {
                int pixel = memory[I + row];
                for(column = 0; column < 8; column++) {
                    if((pixel & (0x80 >> column)) != 0) {
                        if(display[V[x] + column + ((V[y] + row) * 64)] == 1) {
                            V[0xF] = 1;
                        }
                        display[V[x] + column + ((V[y] + row) * 64)] ^= 1;
                    }
                }
            }
        case 0xE000:
            switch(opcode & 0x00FF) {
                case 0x009E:
                case 0x00A1:
            }
        case 0xF000:
            switch(opcode & 0x00FF) {
                case 0x0007:
                case 0x000A:
                case 0x0015:
                case 0x0018:
                case 0x001E:
                case 0x0029:
                case 0x0033:
                case 0x0055:
                case 0x0065:
                default:
                    break;
            }
        default:
            break;
    }
}