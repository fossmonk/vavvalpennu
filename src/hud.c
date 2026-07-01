#include <stdlib.h>
#include <raylib.h>
#include <hud.h>
#include <player.h>
#include <hbar.h>
#include <bbox.h>
#include <textengine.h>

Texture2D k_hud_tex;
Texture2D a_hud_tex;
bbox_t k_hud_bbox;
bbox_t a_hud_bbox;
Font hudfont;

int kpos_x;
int kpos_y;
int apos_x;
int apos_y;

void hud_init(void) {
    int padding = 10;
    k_hud_tex = LoadTexture(K_HUD_TEXTURE);
    a_hud_tex = LoadTexture(A_HUD_TEXTURE);

    kpos_x = G_W - k_hud_tex.width - padding;
    kpos_y = padding;
    apos_x = G_W - a_hud_tex.width - padding;
    apos_y = k_hud_tex.height + padding;

    k_hud_bbox = bbox_parse(K_HUD_BBOX);
    a_hud_bbox = bbox_parse(A_HUD_BBOX);

    hudfont = LoadFontEx(HUD_FONT, 96, NULL, 0);
    SetTextureFilter(hudfont.texture, TEXTURE_FILTER_BILINEAR);
}

void hud_draw(player_t *p) {
    hbar_draw(&p->hbar);
    // karikku count
    DrawTexture(k_hud_tex, kpos_x, kpos_y, WHITE);
    Rectangle rk = k_hud_bbox.bbox.rect;
    rk.x += kpos_x;
    rk.y += kpos_y;
    te_draw_inside_rect((char *)TextFormat("%03d", p->k_count), hudfont, 35, rk);
    // artifact count
    DrawTexture(a_hud_tex, apos_x, apos_y, WHITE);
    Rectangle ra = a_hud_bbox.bbox.rect;
    ra.x += apos_x;
    ra.y += apos_y;
    te_draw_inside_rect((char *)TextFormat("%02d", p->a_count), hudfont, 35, ra);
    DrawTextEx(hudfont, TextFormat("SCORE   : %d", p->score), (Vector2){0, 40}, 35, 1, WHITE);
}