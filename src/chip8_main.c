#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include "chip8_core.h"
#include "chip8_platform.h"

int main(int argc, char **argv) {
    printf("%d\n", argc);
    printf("%s\n", argv[0]);
    if(!init()) {
        fprintf(stderr, "Error while initializing.\n");
        exit(EXIT_FAILURE);
    }
    if(!stateinit()) exit(EXIT_FAILURE);
    init_cpu();
    load_rom("rom/IBM.ch8");
    print_rom_instructions("rom/IBM.ch8");
    clearscreen();
    while(state != QUIT) {
    //    printf("Awaiting input\n");
        handleinputs();
    //    printf("Delay started\n");
        emulate_cycle();
        SDL_Delay(16);
    //    printf("Updating screen\n");
        updatescreen();
    }
    cleanup();
    return 0;
}
