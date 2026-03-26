#pragma once
#include <3ds.h>
#include "chip8.h"

class Input {
public:
    void update(Chip8& chip8);
private:
    // Mapeamento: botoes 3DS -> teclas CHIP-8 (layout hex 0-F)
    static const int keyMap[NUM_KEYS];
};
