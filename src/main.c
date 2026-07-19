#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <vpconfig.h>
#include <rand.h>
#include <game.h>

#ifdef DEBUG
#define SET_TRACE SetTraceLogLevel(LOG_ERROR)
#else
#define SET_TRACE SetTraceLogLevel(LOG_NONE)
#endif

static void set_window(void) {
    /* Init Window with our original dimensions */
    InitWindow(G_W, G_H, G_TITLE);
    /* Find out display resolutions */
    int mon = GetCurrentMonitor();
    int sw = GetMonitorWidth(mon);
    int sh = GetMonitorHeight(mon);

    #ifndef DEBUG
    SetWindowSize(sw, sh);
    SetWindowState(FLAG_FULLSCREEN_MODE);
    #else
    /* Games in fullscreen can't be minimized (raylib issue)
    So for debug, show windowed original size window */
    (void)sw;
    (void)sh;
    SetWindowSize(G_W, G_H);
    #endif
}

int main(void) {
    vp_rand_seed(time(NULL));

    SET_TRACE;

    set_window();
    
    /* No exit keys, handle separately */
    SetExitKey(KEY_NULL);
    /* Hide the cursor since we have custom cursor */
    HideCursor();

    /* Create virtual canvas always with original dims G_W, G_H */
    RenderTexture2D canvas = LoadRenderTexture(G_W, G_H);
    SetTextureFilter(canvas.texture, TEXTURE_FILTER_BILINEAR);

    game_init(&canvas);
    game_start_scene();
    game_fade_into_main();
    game_start_main_loop();
    game_deinit();

    CloseWindow();
    return 0;
}