#include "ui.h"
#include <cstdio>

void UI::init(C3D_RenderTarget* t) {
    target   = t;
    textBuf  = C2D_TextBufNew(256);
}

void UI::draw(bool paused, uint32_t speed, uint16_t pc) {
    C2D_TextBufClear(textBuf);

    u32 accent  = C2D_Color32(0xA8, 0xFF, 0xC8, 0xFF);
    u32 muted   = C2D_Color32(0x88, 0x88, 0xAA, 0xFF);
    u32 white   = C2D_Color32(0xFF, 0xFF, 0xFF, 0xFF);
    u32 yellow  = C2D_Color32(0xFF, 0xE0, 0x66, 0xFF);

    // Titulo
    C2D_Text title;
    C2D_TextParse(&title, textBuf, "3DS CHIP-8 Player");
    C2D_DrawText(&title, C2D_WithColor, 16.0f, 14.0f, 0.5f, 0.55f, 0.55f, accent);

    // Status
    char statusBuf[32];
    snprintf(statusBuf, sizeof(statusBuf), paused ? "[ PAUSADO ]" : "[ EM EXECUCAO ]");
    C2D_Text status;
    C2D_TextParse(&status, textBuf, statusBuf);
    C2D_DrawText(&status, C2D_WithColor, 16.0f, 50.0f, 0.5f, 0.45f, 0.45f,
                 paused ? yellow : white);

    // PC e velocidade
    char infoBuf[64];
    snprintf(infoBuf, sizeof(infoBuf), "PC: 0x%03X   Velocidade: %u ciclos/frame", pc, speed);
    C2D_Text info;
    C2D_TextParse(&info, textBuf, infoBuf);
    C2D_DrawText(&info, C2D_WithColor, 16.0f, 80.0f, 0.5f, 0.40f, 0.40f, muted);

    // Controles
    const char* controls[] = {
        "SELECT  Pausar/Retomar",
        "UP/DOWN Velocidade",
        "START   Sair"
    };
    for (int i = 0; i < 3; i++) {
        C2D_Text ctrl;
        C2D_TextParse(&ctrl, textBuf, controls[i]);
        C2D_DrawText(&ctrl, C2D_WithColor, 16.0f, 130.0f + i * 22.0f, 0.5f,
                     0.38f, 0.38f, muted);
    }
}
