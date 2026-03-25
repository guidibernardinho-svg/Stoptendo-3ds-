#pragma once
#include <citro2d.h>
#include <citro3d.h>
#include <cstdint>

class UI {
public:
    void init(C3D_RenderTarget* target);
    void draw(bool paused, uint32_t speed, uint16_t pc);
private:
    C3D_RenderTarget* target;
    C2D_TextBuf       textBuf;
};
