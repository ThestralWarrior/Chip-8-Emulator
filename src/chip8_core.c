#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <locale.h>
#include <stdbool.h>
#include "chip8_core.h"
#include "chip8_platform.h"

uint8_t V[NUM_REGISTERS] = {0};
uint8_t memory[MEMORY_SIZE] = {0};
uint16_t I;
uint16_t PC;
int8_t SP;
uint16_t opcode;
uint16_t stack[STACK_SIZE] = {0};
uint8_t keypad[TOTAL_KEYS] = {0};
uint8_t display[TOTAL_PIXELS] = {0};
uint8_t delay;
uint8_t sound;
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

uint16_t address;
uint8_t value;
uint8_t x;
uint8_t y;
uint8_t flag;

typedef void (*operation)();
operation operations[16] = {
    op_0000, op_1000, op_2000, op_3000, op_4000, op_5000, op_6000, op_7000,
    op_8000, op_9000, op_A000, op_B000, op_C000, op_D000, op_E000, op_F000
};

void initializeCPU() {
    setlocale(LC_ALL, "");
    printf("CHIP-8 Emulator\n");
    printf("Version: 4.1\n");
    printf("Build Date: %s at %s\n", __DATE__, __TIME__);
#ifdef __GNUC__
    printf("GCC Compiler Version: %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else 
    printf("Compiled with an unknown version.\n");
#endif
    printf("Total Memory Size: %d\n", MEMORY_SIZE);
    printf("Reserved Memory: 0x000 to 0x200\n");
    printf("Memory End: 0xFFF\n");
    printf("Program Memory Start: 0x200\n");
    printf("Number of General-Purpose Registers: 16\n");
    printf("Program Counter: Initialized at 0x200\n");
    PC = 0x200;
    printf("Index Register: Initialized at 0x000\n");
    I = 0;
    printf("Stack Size: %d\n", STACK_SIZE);
    SP = -1;
    printf("Display Resolution: 64 x 32\n");
    printf("Graphics Mode: Monochrome\n");
    printf("Sound Capability: Enabled\n");
    printf("Keypad Layout: 4x4 mapping\n");
    printf("Key Mappings:\n");
    printf("1[1]   2[2]   3[3]   4[C]\n");
    printf("Q[4]   W[5]   E[6]   R[D]\n");
    printf("A[7]   S[8]   D[9]   F[E]\n");
    printf("Z[A]   X[0]   C[B]   V[F]\n");
    printf("SPACE BAR to PAUSE/RESUME\n");
    opcode = 0;
    delay = 0;
    sound = 0;
    srand((unsigned int)time(NULL));
    memcpy(memory, fontset, sizeof(fontset));
}

int loadROM(char *filename) {
    FILE *fptr;
    if((fptr = fopen(filename, "rb")) == NULL) {
        printf("Error while loading rom file. \u2717\n");
        return errno;
    }
    struct stat fileinfo;
    stat(filename, &fileinfo);
    size_t expected_bytes = (size_t)fileinfo.st_size;
    if(expected_bytes > sizeof(memory) - 0x200) {
        printf("Insufficient memory: 3584 accessible bytes available. \u2717\n");
        fclose(fptr);
        return -1;
    }
    size_t bytes_read = fread(memory + 0x200, 1, sizeof(memory) - 0x200, fptr);
    if(bytes_read != expected_bytes) {
        printf("Insufficient bytes: some of the %zu bytes couldn't be read. \u2717\n", bytes_read);
        return -1;
    }
    printf("Bytes read: %ld \u2713\n", bytes_read);
    fclose(fptr);
    return 0;
}

void executeInstruction() {
    opcode = memory[PC] << 8 | memory[PC + 1];
    address = opcode & 0x0FFF;
    value = opcode & 0x00FF;
    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    flag = 0;
    uint8_t opcode_type = opcode >> 12;
    operations[opcode_type]();
    if (delay > 0) delay -= 1;
    if (sound > 0) {
        beepsound();
        sound -= 1;
    }
}

void op_0000() {
    switch(opcode & 0x00FF) {
        case 0x00E0:
            op_0_E0();
            break;
        case 0x00EE:
            op_0_EE();
            break;
        default:
            printf("0x%04X \u2717\n", opcode);
    }
}
void op_1000() {
    printf("1NNN: 0x%04X \u2713\n", opcode);
    PC = address;
}
void op_2000() {
    printf("2NNN: 0x%04X \u2713\n", opcode);
    push(PC);
    PC = address;
}
void op_3000() {
    printf("3XNN: 0x%04X \u2713\n", opcode);
    if(V[x] == value) {
        PC += 2;
    }
    PC += 2;
}
void op_4000() {
    printf("4XNN: 0x%04X \u2713\n", opcode);
    if(V[x] != value) {
        PC += 2;
    }
    PC += 2;
}
void op_5000() {
    printf("5XY0: 0x%04X \u2713\n", opcode);
    if(V[x] == V[y]) {
        PC += 2;
    }
    PC += 2;
}
void op_6000() {
    printf("6XNN: 0x%04X \u2713\n", opcode);
    V[x] = value;
    PC += 2;
}
void op_7000() {
    printf("7XNN: 0x%04X \u2713\n", opcode);
    V[x] = V[x] + value;
    PC += 2;
}
void op_8000() {
    switch(opcode & 0x000F) {
        case 0x0000:
            op_8_0();
            break;
        case 0x0001:
            op_8_1();
            break;
        case 0x0002:
            op_8_2();
            break;
        case 0x0003:
            op_8_3();
            break;
        case 0x0004:
            op_8_4();
            break;
        case 0x0005:
            op_8_5();
            break;
        case 0x0006:
            op_8_6();
            break;
        case 0x0007:
            op_8_7();
            break;
        case 0x000E:
            op_8_E();
            break;
        default:
            printf("0x%04X \u2717\n", opcode);
    }
}
void op_9000() {
    if(V[x] != V[y]) {
        PC += 2;
    }
    PC += 2;
}
void op_A000() {
    printf("ANNN: 0x%04X \u2713\n", opcode);
    I = address;
    PC += 2;
}
void op_B000() {
    printf("BNNN: 0x%04X \u2713\n", opcode);
    PC = address + V[0];
}
void op_C000() {
    printf("CXNN: 0x%04X \u2713\n", opcode);
    int random = rand() % 256;
    V[x] = random & value;
    PC += 2;
}
void op_D000() {
    printf("DXYN: 0x%04X \u2713\n", opcode);
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
}
void op_E000() {
    switch(opcode & 0x00FF) {
        case 0x009E:
            op_E_9E();
            break;
        case 0x00A1:
            op_E_A1();
            break;
        default:
            printf("0x%04X \u2717\n", opcode);
    }
}
void op_F000() {
    switch(opcode & 0x00FF) {
        case 0x0007:
            op_F_07();
            break;
        case 0x000A:
            op_F_0A();
            break;
        case 0x0015:
            op_F_15();
            break;
        case 0x0018:
            op_F_18();
            break;
        case 0x001E:
            op_F_1E();
            break;
        case 0x0029:
            op_F_29();
            break;
        case 0x0033:
            op_F_33();
            break;
        case 0x0055:
            op_F_55();
            break;
        case 0x0065:
            op_F_65();
            break;
        default:
            printf("0x%04X \u2717\n", opcode);
    }
}

void op_0_E0() {
    printf("00E0: 0x%04X \u2713\n", opcode);
    for(int i = 0; i < 64 * 32; i++) {
        display[i] = 0;
    }
    PC += 2;
}
void op_0_EE() {
    printf("00EE: 0x%04X \u2713\n", opcode);
    pop(&PC);
    PC += 2;
}

void op_8_0() {
    printf("8XY0: 0x%04X \u2713\n", opcode);
    V[x] = V[y];
    PC += 2;
}
void op_8_1() {
    printf("8XY1: 0x%04X \u2713\n", opcode);
    V[x] = V[x] | V[y];
    PC += 2;
}
void op_8_2() {
    printf("8XY2: 0x%04X \u2713\n", opcode);
    V[x] = V[x] & V[y];
    PC += 2;
}
void op_8_3() {
    printf("8XY3: 0x%04X \u2713\n", opcode);
    V[x] = V[x] ^ V[y];
    PC += 2;
}
void op_8_4() {
    printf("8XY4: 0x%04X \u2713\n", opcode);
    int sum = V[x] + V[y];
    V[x] = sum % 256;
    if(sum > 255) V[0xF] = 1;
    else V[0xF] = 0;
    PC += 2;
}
void op_8_5() {
    printf("8XY5: 0x%04X \u2713\n", opcode); 
    if(V[x] >= V[y]) flag = 1;
    else flag = 0;
    V[x] = V[x] - V[y];
    V[0xF] = flag;
    PC += 2;
}
void op_8_6() {
    printf("8XY6: 0x%04X \u2713\n", opcode);
    flag = V[x] & 0x1;
    V[x] = V[x] >> 1;
    V[0xF] = flag;
    PC += 2;
}
void op_8_7() {
    printf("8XY7: 0x%04X \u2713\n", opcode);
    if(V[y] >= V[x]) flag = 1;
    else flag = 0;
    V[x] = V[y] - V[x];
    V[0xF] = flag;
    PC += 2;
}
void op_8_E() {
    printf("8XYE: 0x%04X \u2713\n", opcode);
    flag = (V[x] & 0x80) >> 7;
    V[x] = V[x] << 1;
    V[0xF] = flag;
    PC += 2;
}

void op_E_9E() {
    printf("EX9E: 0x%04X \u2713\n", opcode);
    if(keypad[V[x]]) {
        PC += 2;
    }
    PC += 2;
}
void op_E_A1() {
    printf("EXA1: 0x%04X \u2713\n", opcode);
    if(!keypad[V[x]]) {
        PC += 2;
    }
    PC += 2;
}

void op_F_07() {
    printf("FX07: 0x%04X \u2713\n", opcode);
    V[x] = delay;
    PC += 2;
}
void op_F_0A() {
    printf("FX0A: 0x%04X \u2713\n", opcode);
    for(int i = 0; i < 16; i++) {
        if(keypad[i]) {
            V[x] = i;
            PC += 2;
            break;
        }
    }
}
void op_F_15() {
    printf("FX15: 0x%04X \u2713\n", opcode);
    delay = V[x];
    PC += 2;
}
void op_F_18() {
    printf("FX18: 0x%04X \u2713\n", opcode);
    sound = V[x];
    PC += 2;
}
void op_F_1E() {
    printf("FX1E: 0x%04X \u2713\n", opcode);
    I = I + V[x];
    PC += 2;
}
void op_F_29() {
    printf("FX29: 0x%04X \u2713\n", opcode);
    I = V[x] * 5;
    PC += 2;
}
void op_F_33() {
    printf("FX33: 0x%04X \u2713\n", opcode);
    int ones = V[x] % 10, tens = (V[x] / 10) % 10, hundreds = (V[x] / 100) % 10;
    memory[I] = hundreds;
    memory[I + 1] = tens;
    memory[I + 2] = ones;
    PC += 2;
}
void op_F_55() {
    printf("FX55: 0x%04X \u2713\n", opcode);
    for(int i = 0; i <= x; i++) {
        memory[I + i] = V[i];
    }
    PC += 2;
}
void op_F_65() {
    printf("FX65: 0x%04X \u2713\n", opcode);
    for(int i = 0; i <= x; i++) {
        V[i] = memory[I + i];
    }
    PC += 2;
}

void printInstructions(char *filename) {
    FILE *fptr = NULL;
    if((fptr = fopen(filename, "rb")) == NULL) {
        printf("Couldn't load rom file. \u2717\n");
        return;
    }
    uint16_t op;
    printf("Instructions in %s: ", filename);
    while(fread(&op, sizeof(uint16_t), 1, fptr) == 1) {
        op = ((op & 0xFF) << 8) | ((op & 0xFF00) >> 8);
        printf("0x%04X ", op);
    }
    printf("\n");
}

void push(uint16_t value) {
    if(SP == STACK_SIZE - 1) {
        printf("Stack overflow: exceeding capacity of 16 \u2717\n");
        return;
    }
    stack[++SP] = value;
}

void pop(uint16_t* destination) {
    if(SP == -1) {
        printf("Stack underflow: stack has no contents \u2717\n");
        return;
    }
    *destination = stack[SP--];
}