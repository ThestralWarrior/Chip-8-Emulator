#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "chip8_core.h"
#include "chip8_platform.h"

struct timeval start_time, end_time;

int main(int argc, char **argv) {
    printf("%d\n", argc);
    printf("%s\n", argv[0]);
    char rom[100] = "roms/";
    char *file;
    if(argc < 2) {
        printf("Not enough arguments. Passing IBM.ch8 as program.\n");
        file = "IBM.ch8";
    } else {
        file = argv[1];
    }

    printf("Loading rom: %s\n", rom);
    strcat(rom, file);
    printf("%s\n", rom);
    if(!init()) {
        fprintf(stderr, "Error while initializing.\n");
        exit(EXIT_FAILURE);
    }
    if(!stateinit()) exit(EXIT_FAILURE);
    initializeCPU();
    loadROM(rom);
    printInstructions(rom);
    clearscreen();
    while(state != QUIT) {
        handleinputs();
        if(state == PAUSED) continue;
        gettimeofday(&start_time, NULL);
        executeInstruction();
        gettimeofday(&end_time, NULL);
        uint32_t timeInMillis = (uint32_t)((double)(end_time.tv_sec - start_time.tv_sec) + (double)(end_time.tv_usec - start_time.tv_usec) / 1000000.0) * 1000;
        SDL_Delay(16 - timeInMillis);
        updatescreen();
    }
    cleanup();
    return 0;
}
