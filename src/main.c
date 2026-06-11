#include <stdio.h>
#include <raylib.h>
#include <vpconfig.h>

#include <game.h>

int main(void) {
    // Init Raylib window with current monitor dimensions
    int monitor = GetCurrentMonitor();
    int monitor_w = GetMonitorWidth(monitor);
    int monitor_h = GetMonitorHeight(monitor);
    InitWindow(monitor_w, monitor_h, G_TITLE);
    // No exit keys, handle separately.
    SetExitKey(KEY_NULL);
    // Start game in full screen mode
    SetWindowState(FLAG_FULLSCREEN_MODE);

    // Create virtual canvas with G_W, G_H
    RenderTexture2D canvas = LoadRenderTexture(G_W, G_H);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);

    game_init(&canvas);
    game_start_scene();
    game_start_main_loop();

    CloseWindow();
    return 0;
}