#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include "chip8_platform.h"
#include "chip8_core.h"

emulator_state state = QUIT;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

// SDL_Scancode keymappings[16] = {
//     SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
//     SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
//     SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_F,
//     SDL_SCANCODE_Z, SDL_SCANCODE_X, SDL_SCANCODE_C, SDL_SCANCODE_V};

bool init() {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0) {
        SDL_Log("Error while initializing: %s\n", SDL_GetError());
        return false;
    }
    window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED ,  SDL_WINDOWPOS_CENTERED, WIDTH * SCALE, HEIGHT * SCALE, 0);
    if(window == NULL) {
        SDL_Log("Window not initialized: %s\n", SDL_GetError());
        return false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(renderer == NULL) {
        SDL_Log("Renderer not initialized: %s\n", SDL_GetError());
        return false;
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
        // const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
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
                    case SDLK_v:
                        keypad[0xF] = 1;
                        printf("F\n");
                        break;
                    case SDLK_f:
                        keypad[0xE] = 1;
                        printf("E\n");
                        break;
                    case SDLK_r:
                        keypad[0xD] = 1;
                        printf("D\n");
                        break;
                    case SDLK_4:
                        keypad[0xC] = 1;
                        printf("C\n");
                        break;
                    case SDLK_c:
                        keypad[0xB] = 1;
                        printf("B\n");
                        break;
                    case SDLK_z:
                        keypad[0xA] = 1;
                        printf("A\n");
                        break;
                    case SDLK_d:
                        keypad[0x9] = 1;
                        printf("9\n");
                        break;
                    case SDLK_s:
                        keypad[0x8] = 1;
                        printf("8\n");
                        break;
                    case SDLK_a:
                        keypad[0x7] = 1;
                        printf("7\n");
                        break;
                    case SDLK_e:
                        keypad[0x6] = 1;
                        printf("6\n");
                        break;
                    case SDLK_w:
                        keypad[0x5] = 1;
                        printf("5\n");
                        break;
                    case SDLK_q:
                        keypad[0x4] = 1;
                        printf("4\n");
                        break;
                    case SDLK_3:
                        keypad[0x3] = 1;
                        printf("3\n");
                        break;
                    case SDLK_2:
                        keypad[0x2] = 1;
                        printf("2\n");
                        break;
                    case SDLK_1:
                        keypad[0x1] = 1;
                        printf("1\n");
                        break;
                    case SDLK_x:
                        keypad[0x0] = 1;
                        printf("0\n");
                        break;
                    default:
                        // for (int keycode = 0; keycode < 16; keycode++) {
                        //     keypad[keycode] = keyboardState[keymappings[keycode]];
                        // }
                        break;
                }
                break;
            case SDL_KEYUP:
                switch(event.key.keysym.sym) {
                    case SDLK_v:
                        keypad[0xF] = 0;
                        break;
                    case SDLK_f:
                        keypad[0xE] = 0;
                        break;
                    case SDLK_r:
                        keypad[0xD] = 0;
                        break;
                    case SDLK_4:
                        keypad[0xC] = 0;
                        break;
                    case SDLK_c:
                        keypad[0xB] = 0;
                        break;
                    case SDLK_z:
                        keypad[0xA] = 0;
                        break;
                    case SDLK_d:
                        keypad[0x9] = 0;
                        break;
                    case SDLK_s:
                        keypad[0x8] = 0;
                        break;
                    case SDLK_a:
                        keypad[0x7] = 0;
                        break;
                    case SDLK_e:
                        keypad[0x6] = 0;
                        break;
                    case SDLK_w:
                        keypad[0x5] = 0;
                        break;
                    case SDLK_q:
                        keypad[0x4] = 0;
                        break;
                    case SDLK_3:
                        keypad[0x3] = 0;
                        break;
                    case SDLK_2:
                        keypad[0x2] = 0;
                        break;
                    case SDLK_1:
                        keypad[0x1] = 0;
                        break;
                    case SDLK_x:
                        keypad[0x0] = 0;
                        break;
                    default:
                        break;
                }
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
}