#ifndef CHIP8_PLATFORM_H
#define CHIP8_PLATFORM_H

#define WIDTH 64
#define HEIGHT 32
#define BGCOLOR 0x00000000 
#define FGCOLOR 0xFFFF00FF
#define SCALE 10

typedef enum {
    QUIT = 0,
    RUNNING,
    PAUSE
} emulator_state;

bool init();
void cleanup();
void clearscreen();
void updatescreen();
void handleinputs();
bool stateinit();

#endif