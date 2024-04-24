# Name of the output binary
TARGET = chip8

# Source file(s)
SOURCES = chip8_platform.c

# Flags for SDL2, obtained from sdl2-config
SDL_FLAGS = $(shell sdl2-config --cflags --libs)

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Rule to compile the project
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES) $(SDL_FLAGS)

# Rule to clean up compiled files
.PHONY: clean
clean:
	rm -f $(TARGET)
