#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<SDL2/SDL.h>
#include "chip8_platform.h"

emulator_state state = QUIT;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

int main(int argc, char **argv) {
    printf("%d\n", argc);
    printf("%s\n", argv[0]);
    if(!init()) {
        fprintf(stderr, "Error while initializing.\n");
        exit(EXIT_FAILURE);
    }
    if(!stateinit()) exit(EXIT_FAILURE);
    clearscreen();
    while(state != QUIT) {
        printf("Awaiting input\n");
        handleinputs();
        printf("Delay started\n");
        SDL_Delay(2000);
        printf("Updating screen\n");
        updatescreen();
    }
    cleanup();
    return 0;
}

bool init() {
    printf("Initializing SDL...\n");
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Error while initializing: %s\n", SDL_GetError());
        return false;
    }
    printf("Creating Window...\n");
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED ,  SDL_WINDOWPOS_CENTERED, WIDTH * SCALE, HEIGHT * SCALE, 0);
    if(window == NULL) {
        SDL_Log("Window not initialized: %s\n", SDL_GetError());
        return false;
    }
    printf("Creating Renderer...\n");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) {
        SDL_Log("Renderer not initialized: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

void cleanup() {
    printf("Destroying Renderer...\n");
    if(renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    printf("Destroying Window...\n");
    if(window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
    printf("Quitting all SDL subsystems...\n");
    SDL_Quit();
}

void clearscreen() {
    uint8_t r = (BGCOLOR >> 24) & 0xFF;
    uint8_t g = (BGCOLOR >> 16) & 0xFF;
    uint8_t b = (BGCOLOR >> 8) & 0xFF;
    uint8_t a = (BGCOLOR >> 4) & 0xFF;
    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
}

void updatescreen() {
    SDL_RenderPresent(renderer);
}

void handleinputs() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                state = QUIT;
                printf("You decided to quit...\n");
                return;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        state = QUIT;
                        return;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                break;
            default:
                break;
        }
    }
}

bool stateinit() {
    state = RUNNING;
    return true;
}