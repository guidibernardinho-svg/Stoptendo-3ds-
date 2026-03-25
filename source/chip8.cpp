#include "chip8.h"
#include <cstdio>

const uint8_t Chip8::fontset[80] = {
    0xF0,0x90,0x90,0x90,0xF0, // 0
    0x20,0x60,0x20,0x20,0x70, // 1
    0xF0,0x10,0xF0,0x80,0xF0, // 2
    0xF0,0x10,0xF0,0x10,0xF0, // 3
    0x90,0x90,0xF0,0x10,0x10, // 4
    0xF0,0x80,0xF0,0x10,0xF0, // 5
    0xF0,0x80,0xF0,0x90,0xF0, // 6
    0xF0,0x10,0x20,0x40,0x40, // 7
    0xF0,0x90,0xF0,0x90,0xF0, // 8
    0xF0,0x90,0xF0,0x10,0xF0, // 9
    0xF0,0x90,0xF0,0x90,0x90, // A
    0xE0,0x90,0xE0,0x90,0xE0, // B
    0xF0,0x80,0x80,0x80,0xF0, // C
    0xE0,0x90,0x90,0x90,0xE0, // D
    0xF0,0x80,0xF0,0x80,0xF0, // E
    0xF0,0x80,0xF0,0x80,0x80  // F
};

void Chip8::init() {
    pc = 0x200;
    I = sp = delayTimer = soundTimer = 0;
    memset(V, 0, sizeof(V));
    memset(memory, 0, sizeof(memory));
    memset(stack, 0, sizeof(stack));
    memset(keys, 0, sizeof(keys));
    clearDisplay();
    for (int i = 0; i < 80; i++)
        memory[0x50 + i] = fontset[i];
    srand((unsigned)time(NULL));
}

bool Chip8::loadROM(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);
    if (size > (MEMORY_SIZE - 0x200)) { fclose(f); return false; }
    fread(&memory[0x200], 1, size, f);
    fclose(f);
    return true;
}

void Chip8::clearDisplay() {
    memset(display, 0, sizeof(display));
}

uint8_t Chip8::randomByte() {
    return (uint8_t)(rand() & 0xFF);
}

void Chip8::updateTimers() {
    if (delayTimer > 0) delayTimer--;
    if (soundTimer > 0) soundTimer--;
}

void Chip8::emulateCycle() {
    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    uint8_t  x   = (opcode & 0x0F00) >> 8;
    uint8_t  y   = (opcode & 0x00F0) >> 4;
    uint8_t  n   =  opcode & 0x000F;
    uint8_t  kk  =  opcode & 0x00FF;
    uint16_t nnn =  opcode & 0x0FFF;

    switch (opcode & 0xF000) {
        case 0x0000:
            if (opcode == 0x00E0) clearDisplay();
            else if (opcode == 0x00EE) { pc = stack[--sp]; }
            break;
        case 0x1000: pc = nnn; break;
        case 0x2000: stack[sp++] = pc; pc = nnn; break;
        case 0x3000: if (V[x] == kk) pc += 2; break;
        case 0x4000: if (V[x] != kk) pc += 2; break;
        case 0x5000: if (V[x] == V[y]) pc += 2; break;
        case 0x6000: V[x] = kk; break;
        case 0x7000: V[x] += kk; break;
        case 0x8000:
            switch (n) {
                case 0x0: V[x]  = V[y]; break;
                case 0x1: V[x] |= V[y]; break;
                case 0x2: V[x] &= V[y]; break;
                case 0x3: V[x] ^= V[y]; break;
                case 0x4: { uint16_t r = V[x]+V[y]; V[0xF]=(r>0xFF)?1:0; V[x]=r&0xFF; break; }
                case 0x5: { V[0xF]=(V[x]>=V[y])?1:0; V[x]-=V[y]; break; }
                case 0x6: { V[0xF]=V[x]&0x1; V[x]>>=1; break; }
                case 0x7: { V[0xF]=(V[y]>=V[x])?1:0; V[x]=V[y]-V[x]; break; }
                case 0xE: { V[0xF]=(V[x]>>7)&0x1; V[x]<<=1; break; }
            } break;
        case 0x9000: if (V[x] != V[y]) pc += 2; break;
        case 0xA000: I = nnn; break;
        case 0xB000: pc = nnn + V[0]; break;
        case 0xC000: V[x] = randomByte() & kk; break;
        case 0xD000: {
            V[0xF] = 0;
            for (int row = 0; row < n; row++) {
                uint8_t sprite = memory[I + row];
                int py = (V[y] + row) % DISPLAY_H;
                for (int col = 0; col < 8; col++) {
                    if (sprite & (0x80 >> col)) {
                        int px = (V[x] + col) % DISPLAY_W;
                        if (display[py][px]) V[0xF] = 1;
                        display[py][px] ^= 1;
                    }
                }
            }
        } break;
        case 0xE000:
            if (kk == 0x9E && keys[V[x]]) pc += 2;
            if (kk == 0xA1 && !keys[V[x]]) pc += 2;
            break;
        case 0xF000:
            switch (kk) {
                case 0x07: V[x] = delayTimer; break;
                case 0x0A: {
                    bool pressed = false;
                    for (int k = 0; k < NUM_KEYS; k++) {
                        if (keys[k]) { V[x] = k; pressed = true; break; }
                    }
                    if (!pressed) pc -= 2;
                } break;
                case 0x15: delayTimer = V[x]; break;
                case 0x18: soundTimer = V[x]; break;
                case 0x1E: I += V[x]; break;
                case 0x29: I = 0x50 + (V[x] * 5); break;
                case 0x33:
                    memory[I]   = V[x] / 100;
                    memory[I+1] = (V[x] / 10) % 10;
                    memory[I+2] = V[x] % 10;
                    break;
                case 0x55: for (int i=0;i<=x;i++) memory[I+i]=V[i]; break;
                case 0x65: for (int i=0;i<=x;i++) V[i]=memory[I+i]; break;
            } break;
    }
}
