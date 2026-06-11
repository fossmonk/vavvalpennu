#include <raylib.h>

#ifndef _HBAR_H_
#define _HBAR_H_

typedef struct {
    int max_inner_w;
    int spacing;
    Rectangle outer_rec;
    Rectangle inner_rec;
    Shader shader;
    Texture icon;
} hbar_t;

void hbar_init(hbar_t *h, Vector2 pos, int max_w, int height, int spacing);
void hbar_update(hbar_t *h, int health, int max_health);
void hbar_draw(hbar_t *h, Texture2D* icon, Vector2 iconpos, Color c);

#endif