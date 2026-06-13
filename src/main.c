#include <stdio.h>
#include <raylib.h>
#include <vpconfig.h>

#include <game.h>

int main(void) {
    // Init Window with our original dimensions
    InitWindow(G_W, G_H, G_TITLE);
    // Find out display resolutions
    int mon = GetCurrentMonitor();
    int sw = GetMonitorWidth(mon);
    int sh = GetMonitorHeight(mon);
    // Now reset the window size to screen size
    SetWindowSize(sw, sh);
    // No exit keys, handle separately
    SetExitKey(KEY_NULL);
    // Start game in full screen mode
    SetWindowState(FLAG_FULLSCREEN_MODE);

    // Create virtual canvas always with original dims G_W, G_H
    RenderTexture2D canvas = LoadRenderTexture(G_W, G_H);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);

    game_init(&canvas);
    game_start_scene();
    game_start_main_loop();

    CloseWindow();
    return 0;
}