#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<SDL2/SDL.h>
#define WIDTH 64
#define HEIGHT 32

bool init();
void close();

int main(int argc, char **argv) {
    if(!init()) {
        fprintf(stderr, "Error while initializing.");
        exit(EXIT_FAILURE);
    }
}

bool init() {
    return true;
}

void close(void) {
    
}