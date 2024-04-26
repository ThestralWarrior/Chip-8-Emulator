# Name of the output binary
TARGET = chip8

# Source file(s)
SOURCES = src/chip8_main.c src/chip8_platform.c src/chip8_core.c

# Include path(s)
INCLUDE_PATHS = -Iinclude

# Flags for SDL2, obtained from sdl2-config
SDL_FLAGS = $(shell sdl2-config --cflags --libs)

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra $(INCLUDE_PATHS)

# Rule to compile the project
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(SDL_FLAGS)

# Rule to clean up compiled files
.PHONY: clean
clean:
	rm -f $(TARGET)
