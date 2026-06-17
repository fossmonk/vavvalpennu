#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <batr.h>
#include <hbar.h>

#ifndef _PLAYER_H_
#define _PLAYER_H_

// action bitmasks
#define IS_DYING     (1 << 0)
#define IS_JUMPING   (1 << 1)
#define IS_HURTING_F (1 << 2)
#define IS_HURTING_S (1 << 3)
#define IS_HURTING_B (1 << 4)

#define player_is_jumping(p) (((p)->actionmask & IS_JUMPING) == IS_JUMPING)
#define player_is_dying(p) (((p)->actionmask & IS_DYING) == IS_DYING)
#define player_is_hurting(p) ((p)->actionmask & (IS_HURTING_F | IS_HURTING_S | IS_HURTING_B))

#define player_set_die(p)        ((p)->actionmask |= IS_DYING)
#define player_set_jump(p)       ((p)->actionmask |= IS_JUMPING)
#define player_set_hurt_flash(p) ((p)->actionmask |= IS_HURTING_F)
#define player_set_hurt_shock(p) ((p)->actionmask |= IS_HURTING_S)
#define player_set_hurt_burn(p)  ((p)->actionmask |= IS_HURTING_B)

#define player_clr_die(p)        ((p)->actionmask &= ~IS_DYING)
#define player_clr_jump(p)       ((p)->actionmask &= ~IS_JUMPING)
#define player_clr_hurt_flash(p) ((p)->actionmask &= ~IS_HURTING_F)
#define player_clr_hurt_shock(p) ((p)->actionmask &= ~IS_HURTING_S)
#define player_clr_hurt_burn(p)  ((p)->actionmask &= ~IS_HURTING_B)

typedef struct {
    obj_t obj;
    int health;
    int max_health;
    hbar_t hbar;
    Texture2D hbar_icon;
    Vector2 hbar_iconpos;
    unsigned char actionmask;
    bool in_hurt_anim;

    // animations
    anim_t anim_run_r;
    anim_t anim_idle_r;
    anim_t anim_jump_r;
    anim_t anim_hithurt;
    anim_t anim_flash;
    anim_t anim_shock;
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