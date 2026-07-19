#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <batr.h>
#include <hbar.h>
#include <levels.h>

#ifndef _PLAYER_H_
#define _PLAYER_H_

#define PLAYER_HITBOX_RB (5.0f)

#define PLAYER_ORB_HEALTH_DECR (20)
#define PLAYER_SKBALL_HEALTH_DECR (5)

// action bitmasks
#define P_IS_DYING     (1 << 0)
#define P_IS_JUMPING   (1 << 1)
#define P_IS_HURTING_F (1 << 2)
#define P_IS_HURTING_S (1 << 3)
#define P_IS_HURTING_B (1 << 4)
#define P_IS_DANCING   (1 << 5)

#define player_is_jumping(p) (((p)->actionmask & P_IS_JUMPING) == P_IS_JUMPING)
#define player_is_dying(p) (((p)->actionmask & P_IS_DYING) == P_IS_DYING)
#define player_is_dancing(p) (((p)->actionmask & P_IS_DANCING) == P_IS_DANCING)
#define player_is_hurting(p) ((p)->actionmask & (P_IS_HURTING_F | P_IS_HURTING_S | P_IS_HURTING_B))

#define player_set_die(p)        ((p)->actionmask |= P_IS_DYING)
#define player_set_jump(p)       ((p)->actionmask |= P_IS_JUMPING)
#define player_set_hurt_flash(p) ((p)->actionmask |= P_IS_HURTING_F)
#define player_set_hurt_shock(p) ((p)->actionmask |= P_IS_HURTING_S)
#define player_set_hurt_burn(p)  ((p)->actionmask |= P_IS_HURTING_B)

#define player_clr_die(p)        ((p)->actionmask &= ~P_IS_DYING)
#define player_clr_jump(p)       ((p)->actionmask &= ~P_IS_JUMPING)
#define player_clr_hurt_flash(p) ((p)->actionmask &= ~P_IS_HURTING_F)
#define player_clr_hurt_shock(p) ((p)->actionmask &= ~P_IS_HURTING_S)
#define player_clr_hurt_burn(p)  ((p)->actionmask &= ~P_IS_HURTING_B)

#define player_decr_health_skball(p) player_decr_health((p), PLAYER_SKBALL_HEALTH_DECR)
#define player_decr_health_orb(p) player_decr_health((p), PLAYER_ORB_HEALTH_DECR)

typedef struct {
    Sound whip;
    Sound jump;
    Sound slurp;
    Sound batr_whoosh;
    Sound hurt;
    Sound potion;
    Sound theme;
} player_sounds_t;

typedef struct {
    anim_t run;
    anim_t idle;
    anim_t jump;
    anim_t hithurt;
    anim_t flash;
    anim_t shock;
    anim_t wlash;
} player_anims_t;

typedef struct {
    obj_t obj;
    // animations
    player_anims_t anims;
    // sounds
    player_sounds_t sounds;
    // misc
    Vector2 prev_pos;
    vplevel curr_level;
    hbar_t hbar;
    Texture2D hbar_icon;
    Vector2 hbar_iconpos;
    // obstacle array: contains pointers to obstacle objects
    obj_t **obstacle_list;
    int health;
    int max_health;
    int k_count;
    int a_count;
    int score;
    unsigned char actionmask;
    bool in_hurt_anim;
} player_t;

void player_init(player_t *p, obj_t **obstacle_list);
void player_draw(player_t *p);
bool player_can_level_up(player_t *p);
void player_activate_hmove(player_t *p, hdir_t hdir, float dt);
void player_activate_jump(player_t *p, float dt);
void player_activate_hurting(player_t *p, float dt);
void player_activate_whiplash(player_t *p, Vector2 mouse_pos);
void player_activate_batr(player_t *p, batr_t *b, Vector2 pos);
void player_decr_health(player_t *p, int amount);
void player_clamp_to_screenx(player_t *p);
void player_update(player_t *p, float dt);

#endif