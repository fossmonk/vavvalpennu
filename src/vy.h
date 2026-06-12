#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <hbar.h>

#ifndef _VY_H_
#define _VY_H_

typedef struct {
    obj_t obj;
    int health;
    int max_health;
    hbar_t hbar;
    Texture2D hbar_icon;
    Vector2 hbar_iconpos;
    bool is_dying;
    bool is_jumping;
    bool is_orbpos;

    // animations
    anim_t anim_vy_rise;
} vy_t;

void vy_init(vy_t *vy);

static inline Vector2 vy_get_initial_spos(void) {
    return (Vector2){G_W-300, GAME_GROUND_Y-250};
}

static inline Vector2 vy_get_final_spos(void) {
    return (Vector2){G_W/2 - 150, 40};
}

#endif