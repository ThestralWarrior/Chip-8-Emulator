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
extern uint8_t SP;
extern uint16_t opcode;
extern uint16_t stack[STACK_SIZE];
extern uint8_t keypad[TOTAL_KEYS];
extern uint8_t display[TOTAL_PIXELS];
extern uint8_t delay;
extern uint8_t sound;
extern const uint8_t fontset[NUM_FONTS];
extern uint8_t drawFlag;
extern uint8_t soundFlag;

void init_cpu();
int load_rom(char *filename);
void emulate_cycle();

#endif