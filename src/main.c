#include <stdio.h>
#include <raylib.h>
#include <vpconfig.h>

#include <game.h>

int main(void) {
    InitWindow(G_W, G_H, G_TITLE);
    game_init();
    game_start_scene();
    game_start_main_loop();

    CloseWindow();
    return 0;
}