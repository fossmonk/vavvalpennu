#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <batr.h>
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
    bool is_hurting;

    // animations
    anim_t anim_run_r;
    anim_t anim_idle_r;
    anim_t anim_jump_r;
    anim_t anim_hithurt;
} player_t;

void player_init(player_t *p);
void player_draw(player_t *p);
void player_activate_move_r(player_t *p, float dt);
void player_activate_move_l(player_t *p, float dt);
void player_activate_jump(player_t *p, float dt);
void player_activate_hurting(player_t *p, float dt);
void player_activate_batr(player_t *p, batr_t *b, Vector2 pos, float dt);
void player_update(player_t *p, bool boss_active, float dt);

#endif