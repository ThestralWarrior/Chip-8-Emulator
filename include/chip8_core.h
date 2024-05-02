#ifndef CHIP8_CORE_H
#define CHIP8_CORE_H

#include <stdint.h>

#define NUM_REGISTERS 16
#define MEMORY_SIZE 4096
#define STACK_SIZE 16
#define TOTAL_KEYS 16
#define TOTAL_PIXELS 64 * 32
#define NUM_FONTS 80

extern uint8_t V[NUM_REGISTERS];
extern uint8_t memory[MEMORY_SIZE];
extern uint16_t I;
extern uint16_t PC;
extern int8_t SP;
extern uint16_t opcode;
extern uint16_t stack[STACK_SIZE];
extern uint8_t keypad[TOTAL_KEYS];
extern uint8_t display[TOTAL_PIXELS];
extern uint8_t delay;
extern uint8_t sound;
extern const uint8_t fontset[NUM_FONTS];
extern uint8_t drawFlag;
extern uint8_t soundFlag;

void op_0000();
void op_1000();
void op_2000();
void op_3000();
void op_4000();
void op_5000();
void op_6000();
void op_7000();
void op_8000();
void op_9000();
void op_A000();
void op_B000();
void op_C000();
void op_D000();
void op_E000();
void op_F000();

void op_0_E0();
void op_0_EE();

void op_8_0();
void op_8_1();
void op_8_2();
void op_8_3();
void op_8_4();
void op_8_5();
void op_8_6();
void op_8_7();
void op_8_E();

void op_E_9E();
void op_E_A1();

void op_F_07();
void op_F_0A();
void op_F_15();
void op_F_18();
void op_F_1E();
void op_F_29();
void op_F_33();
void op_F_55();
void op_F_65();

void initializeCPU();
int loadROM(char *filename);
void executeInstruction();
void printInstructions(char *filename);

void push(uint16_t value);
void pop(uint16_t* destination);

#endif