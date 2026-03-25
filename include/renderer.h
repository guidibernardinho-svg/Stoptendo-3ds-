#pragma once
#include <citro2d.h>
#include <citro3d.h>
#include "chip8.h"

class Renderer {
public:
    void init(C3D_RenderTarget* target);
    void drawDisplay(bool display[][DISPLAY_W], int screenW, int screenH);
private:
    C3D_RenderTarget* target;
    float cellW, cellH;
    float offsetX, offsetY;
};
