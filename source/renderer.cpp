#include "renderer.h"

void Renderer::init(C3D_RenderTarget* t) {
    target = t;
    // Calcula celulas para centralizar o display CHIP-8 (64x32) na tela superior (400x240)
    float maxW = 400.0f - 32.0f;
    float maxH = 240.0f - 32.0f;
    float scaleX = maxW / DISPLAY_W;
    float scaleY = maxH / DISPLAY_H;
    float scale  = (scaleX < scaleY) ? scaleX : scaleY;
    cellW   = scale;
    cellH   = scale;
    offsetX = (400.0f - cellW * DISPLAY_W) / 2.0f;
    offsetY = (240.0f - cellH * DISPLAY_H) / 2.0f;
}

void Renderer::drawDisplay(bool display[][DISPLAY_W], int screenW, int screenH) {
    (void)screenW; (void)screenH;
    u32 colorOn  = C2D_Color32(0xA8, 0xFF, 0xC8, 0xFF); // verde fosforescente
    u32 colorOff = C2D_Color32(0x0D, 0x0D, 0x1A, 0xFF); // fundo escuro

    for (int y = 0; y < DISPLAY_H; y++) {
        for (int x = 0; x < DISPLAY_W; x++) {
            float px = offsetX + x * cellW;
            float py = offsetY + y * cellH;
            u32 color = display[y][x] ? colorOn : colorOff;
            C2D_DrawRectSolid(px, py, 0.5f, cellW - 1.0f, cellH - 1.0f, color);
        }
    }
}
#include "renderer.h"

void Renderer::init(C3D_RenderTarget* t) {
    target = t;
    // Calcula celulas para centralizar o display CHIP-8 (64x32) na tela superior (400x240)
    float maxW = 400.0f - 32.0f;
    float maxH = 240.0f - 32.0f;
    float scaleX = maxW / DISPLAY_W;
    float scaleY = maxH / DISPLAY_H;
    float scale  = (scaleX < scaleY) ? scaleX : scaleY;
    cellW   = scale;
    cellH   = scale;
    offsetX = (400.0f - cellW * DISPLAY_W) / 2.0f;
    offsetY = (240.0f - cellH * DISPLAY_H) / 2.0f;
}

void Renderer::drawDisplay(bool display[][DISPLAY_W], int screenW, int screenH) {
    (void)screenW; (void)screenH;
    u32 colorOn  = C2D_Color32(0xA8, 0xFF, 0xC8, 0xFF); // verde fosforescente
    u32 colorOff = C2D_Color32(0x0D, 0x0D, 0x1A, 0xFF); // fundo escuro

    for (int y = 0; y < DISPLAY_H; y++) {
        for (int x = 0; x < DISPLAY_W; x++) {
            float px = offsetX + x * cellW;
            float py = offsetY + y * cellH;
            u32 color = display[y][x] ? colorOn : colorOff;
            C2D_DrawRectSolid(px, py, 0.5f, cellW - 1.0f, cellH - 1.0f, color);
        }
    }
}
