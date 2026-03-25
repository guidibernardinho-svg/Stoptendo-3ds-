#include <3ds.h>
#include <citro2d.h>
#include <citro3d.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "chip8.h"
#include "renderer.h"
#include "input.h"
#include "ui.h"

#define TOP_W  400
#define TOP_H  240
#define BOT_W  320
#define BOT_H  240

int main(int argc, char* argv[])
{
    romfsInit();
    gfxInitDefault();
    C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
    C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
    C2D_Prepare();

    C3D_RenderTarget* topScreen = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
    C3D_RenderTarget* botScreen = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);

    Chip8    chip8;
    Renderer renderer;
    Input    input;
    UI       ui;

    chip8.init();
    renderer.init(topScreen);
    ui.init(botScreen);

    // Tenta carregar ROM de romfs; user pode colocar ROMs em SD
    if (!chip8.loadROM("romfs:/roms/test.ch8")) {
        consoleInit(GFX_BOTTOM, NULL);
        printf("\x1b[5;1HFalha ao carregar ROM.\n");
        printf("\x1b[7;1HColoque ROMs .ch8 em:\n");
        printf("\x1b[8;1H/3ds/3ds-player/roms/\n");
        printf("\x1b[10;1HSTART para sair.");
        while (aptMainLoop()) {
            hidScanInput();
            if (hidKeysDown() & KEY_START) break;
            gspWaitForVBlank();
        }
        goto cleanup;
    }

    {
        u32  ticksPerFrame = 10;
        bool paused        = false;

        while (aptMainLoop()) {
            hidScanInput();
            u32 kDown = hidKeysDown();

            if (kDown & KEY_START)  break;
            if (kDown & KEY_SELECT) paused = !paused;
            if (kDown & KEY_UP   )  ticksPerFrame = (ticksPerFrame < 30) ? ticksPerFrame + 1 : 30;
            if (kDown & KEY_DOWN )  ticksPerFrame = (ticksPerFrame >  1) ? ticksPerFrame - 1 : 1;

            if (!paused) {
                input.update(chip8);
                for (u32 i = 0; i < ticksPerFrame; i++)
                    chip8.emulateCycle();
                chip8.updateTimers();
            }

            C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
            C2D_TargetClear(topScreen, C2D_Color32(0x1A, 0x1A, 0x2E, 0xFF));
            C2D_SceneBegin(topScreen);
            renderer.drawDisplay(chip8.getDisplay(), TOP_W, TOP_H);

            C2D_TargetClear(botScreen, C2D_Color32(0x16, 0x13, 0x28, 0xFF));
            C2D_SceneBegin(botScreen);
            ui.draw(paused, ticksPerFrame, chip8.getPC());
            C3D_FrameEnd(0);
        }
    }

cleanup:
    C2D_Fini();
    C3D_Fini();
    gfxExit();
    romfsExit();
    return 0;
}
