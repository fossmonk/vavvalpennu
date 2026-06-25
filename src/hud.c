#include <raylib.h>
#include <hud.h>
#include <player.h>
#include <hbar.h>

void hud_draw(Texture2D ktex, player_t *p) {
    hbar_draw(&p->hbar);
    DrawTexture(ktex, 0, 60, WHITE);
    DrawText(TextFormat(" x %d", p->k_count), ktex.width, 70, 20, WHITE);
    DrawText(TextFormat("SCORE   : %d", p->score), 0, 100, 20, WHITE);
}