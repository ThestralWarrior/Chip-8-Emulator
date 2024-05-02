#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "chip8_platform.h"
#include "chip8_core.h"

emulator_state state = QUIT;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_AudioSpec beepSpec;
SDL_AudioDeviceID beepDevice;

SDL_Scancode keymappings[16] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
    SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V};

bool init() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Error while initializing: %s \u2717\n", SDL_GetError());
        return false;
    }
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED ,  SDL_WINDOWPOS_CENTERED, WIDTH * SCALE, HEIGHT * SCALE, 0);
    if(window == NULL) {
        SDL_Log("Window not initialized: %s \u2717\n", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) {
        SDL_Log("Renderer not initialized: %s \u2717\n", SDL_GetError());
        return false;
    }
    beepSpec.freq = 44100;
    beepSpec.format = AUDIO_U8;
    beepSpec.channels = 1;
    beepSpec.samples = 4096;
    beepSpec.callback = NULL;
    
    beepDevice = SDL_OpenAudioDevice(NULL, 0, &beepSpec, NULL, 0);
    if (beepDevice == 0) {
        printf("Failed to open audio: %s \u2717\n", SDL_GetError());
        return;
    }

    return true;
}

void cleanup() {
    if(renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = NULL;
    }
    if(window) {
        SDL_DestroyWindow(window);
        window = NULL;
    }
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
    SDL_Rect rect;

    uint8_t r_fg = (FGCOLOR >> 24) & 0xFF;
    uint8_t g_fg = (FGCOLOR >> 16) & 0xFF;
    uint8_t b_fg = (FGCOLOR >> 8) & 0xFF;
    uint8_t a_fg = (FGCOLOR >> 0) & 0xFF;

    uint8_t r_bg = (BGCOLOR >> 24) & 0xFF;
    uint8_t g_bg = (BGCOLOR >> 16) & 0xFF;
    uint8_t b_bg = (BGCOLOR >> 8) & 0xFF;
    uint8_t a_bg = (BGCOLOR >> 0) & 0xFF;

    for(int y = 0; y < 32; y++) {
        for(int x = 0; x < 64; x++) {
            if(display[x + (y * 64)]) {
                rect.x = x * SCALE;
                rect.y = y * SCALE;
                rect.w = SCALE;
                rect.h = SCALE;
                SDL_SetRenderDrawColor(renderer, r_fg, g_fg, b_fg, a_fg);
                SDL_RenderFillRect(renderer, &rect);
            } else {
                rect.x = x * SCALE;
                rect.y = y * SCALE;
                rect.w = SCALE;
                rect.h = SCALE;
                SDL_SetRenderDrawColor(renderer, r_bg, g_bg, b_bg, a_bg);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
    SDL_RenderPresent(renderer);
}

void handleinputs() {
    SDL_Event event;
    if(SDL_PollEvent(&event)) {
        const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
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
                    case SDLK_SPACE:
                        if(state == RUNNING) {
                            state = PAUSED;
                            printf("PAUSED\n");
                        } else {
                            state = RUNNING;
                            printf("RESUMED\n");
                        }
                        break;
                    default:
                        for (int keycode = 0; keycode < 16; keycode++) {
                            keypad[keycode] = keyboardState[keymappings[keycode]];
                        }
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

void beepsound() {
    printf("BEEP\n");
    uint8_t beepDuration = 255;
    SDL_QueueAudio(beepDevice, (uint8_t*)"\xFF", 1);
    SDL_PauseAudioDevice(beepDevice, 0);
    SDL_Delay(beepDuration);
}