#include <stdio.h>
#include <raylib.h>
#include <vpconfig.h>

#include <game.h>

int main(void) {
    int monitor = GetCurrentMonitor();
    int monitor_w = GetMonitorWidth(monitor);
    int monitor_h = GetMonitorHeight(monitor);
    InitWindow(monitor_w, monitor_h, G_TITLE);

    // Create virtual canvas with G_W, G_H
    RenderTexture2D canvas = LoadRenderTexture(G_W, G_H);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);

    SetWindowState(FLAG_FULLSCREEN_MODE);
    game_init(&canvas);
    game_start_scene();
    game_start_main_loop();

    CloseWindow();
    return 0;
}