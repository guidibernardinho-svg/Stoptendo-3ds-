#pragma once
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <ctime>

#define DISPLAY_W 64
#define DISPLAY_H 32
#define MEMORY_SIZE 4096
#define STACK_SIZE  16
#define NUM_REGS    16
#define NUM_KEYS    16

class Chip8 {
public:
    void init();
    bool loadROM(const char* path);
    void emulateCycle();
    void updateTimers();

    bool     getDisplay(int x, int y) const { return display[y][x]; }
    bool     (*getDisplay())[DISPLAY_W] { return display; }
    uint16_t getPC() const { return pc; }
    void     setKey(int key, bool pressed) { if(key>=0&&key<NUM_KEYS) keys[key]=pressed; }

private:
    uint8_t  memory[MEMORY_SIZE];
    uint8_t  V[NUM_REGS];
    uint16_t I;
    uint16_t pc;
    uint16_t stack[STACK_SIZE];
    uint8_t  sp;
    uint8_t  delayTimer;
    uint8_t  soundTimer;
    bool     keys[NUM_KEYS];
    bool     display[DISPLAY_H][DISPLAY_W];

    static const uint8_t fontset[80];
    void clearDisplay();
    uint8_t randomByte();
};
