#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <hbar.h>

#ifndef _PLAYER_H_
#define _PLAYER_H_

typedef struct {
    obj_t obj;
    int health;
    int max_health;
    hbar_t hbar;
    Texture2D hbar_icon;
    Vector2 hbar_iconpos;
    bool is_dying;
    bool is_jumping;

    // animations
    anim_t anim_run_r;
    anim_t anim_idle_r;
    anim_t anim_jump_r;
} player_t;

void player_init(player_t *p);

#endif