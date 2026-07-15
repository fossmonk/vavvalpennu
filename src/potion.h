#ifndef _POTION_H_
#define _POTION_H_

#include <raylib.h>
#include <obj.h>

typedef struct {
    obj_t obj;
    Texture2D tex;
    bool is_won;
    float terminal_y;
} potion_t;

void potion_init(potion_t *potion);
void potion_activate_at(potion_t *potion, Vector2 pos, float dt);
void potion_update(potion_t *potion, float dt);
void potion_draw(potion_t *potion);

#endif