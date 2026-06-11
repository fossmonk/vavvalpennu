#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <hbar.h>

void hbar_init(hbar_t *h, Vector2 pos, int max_w, int height, int spacing) {
    // load shader
    h->shader = LoadShader(NULL, HBAR_SHADER);
    h->spacing = spacing;
    h->outer_rec.width = max_w;
    h->outer_rec.height = height;
    h->outer_rec.x = pos.x;
    h->outer_rec.y = pos.y;
    
    h->max_inner_w = max_w - 2*h->spacing;
    h->inner_rec.x = pos.x + h->spacing;
    h->inner_rec.y = pos.y + h->spacing;
    h->inner_rec.height = height - 2*h->spacing;
    h->inner_rec.width = h->max_inner_w;
    int rec_topy_loc = GetShaderLocation(h->shader, "u_topY");
    int rec_height_loc = GetShaderLocation(h->shader, "u_recH");
    int win_height_loc = GetShaderLocation(h->shader, "u_winHeight");
    float win_h = G_H;
    SetShaderValue(h->shader, rec_topy_loc, &h->inner_rec.y, SHADER_UNIFORM_FLOAT);
    SetShaderValue(h->shader, rec_height_loc, &h->inner_rec.height, SHADER_UNIFORM_FLOAT);
    SetShaderValue(h->shader, win_height_loc, &win_h, SHADER_UNIFORM_FLOAT);
}

void hbar_update(hbar_t *h, int health, int max_health) {
    int w = (health * h->max_inner_w) / max_health;
    h->inner_rec.width = w;
}

void hbar_draw(hbar_t *h) {
    DrawRectangleRounded(h->outer_rec, 1, 5, RAYWHITE);
    BeginShaderMode(h->shader);
    DrawRectangleRounded(h->inner_rec, 1, 5, RED);
    EndShaderMode();
}