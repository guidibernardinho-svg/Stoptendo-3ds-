#include "input.h"

// Layout do teclado CHIP-8 original:
// 1 2 3 C      mapeado para botoes 3DS
// 4 5 6 D
// 7 8 9 E
// A 0 B F

const int Input::keyMap[NUM_KEYS] = {
    KEY_X,      // 0
    KEY_A,      // 1
    KEY_B,      // 2
    KEY_Y,      // 3
    KEY_DLEFT,  // 4
    KEY_DUP,    // 5
    KEY_DRIGHT, // 6
    KEY_L,      // 7
    KEY_DDOWN,  // 8
    KEY_R,      // 9
    KEY_ZL,     // A
    KEY_ZR,     // B
    KEY_CSTICK_LEFT,  // C
    KEY_CSTICK_UP,    // D
    KEY_CSTICK_RIGHT, // E
    KEY_CSTICK_DOWN   // F
};

void Input::update(Chip8& chip8) {
    u32 held = hidKeysHeld();
    for (int i = 0; i < NUM_KEYS; i++) {
        chip8.setKey(i, (held & keyMap[i]) != 0);
    }
}
