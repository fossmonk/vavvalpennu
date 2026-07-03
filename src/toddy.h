#ifndef _TODDY_H_
#define _TODDY_H_

#include <raylib.h>
#include <obj.h>

typedef struct {
    obj_t obj;
    Texture2D toddy_tex;
    bool is_won;
    float terminal_y;
} toddy_t;

void toddy_init(toddy_t *toddy);
void toddy_activate_at(toddy_t *toddy, Vector2 pos, float dt);
void toddy_update(toddy_t *toddy, float dt);
void toddy_draw(toddy_t *toddy);

#endif