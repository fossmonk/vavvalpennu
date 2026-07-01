#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <hbar.h>
#include <orb.h>

#ifndef _VY_H_
#define _VY_H_

// action bitmasks
#define VY_IS_DYING     (1 << 0)
#define VY_IS_HURTING_S (1 << 1)

#define vy_is_dying(vy)   ((vy)->actionmask & VY_IS_DYING)
#define vy_is_hurting(vy) ((vy)->actionmask & VY_IS_HURTING_S)

#define vy_set_die(vy)        ((vy)->actionmask |= VY_IS_DYING)
#define vy_set_hurt_shock(vy) ((vy)->actionmask |= VY_IS_HURTING_S)

#define vy_clr_die(vy)        ((vy)->actionmask &= ~VY_IS_DYING)
#define vy_clr_hurt_shock(vy) ((vy)->actionmask &= ~VY_IS_HURTING_S)

typedef struct {
    obj_t obj;
    // animations
    anim_t anim_vy_rise;
    anim_t anim_vy_shock;
    // audio
    Sound hurt;
    Music laugh;
    // weapons
    orb_t orbs[MAX_ORBS];

    // misc
    hbar_t hbar;
    Texture2D hbar_icon;
    Vector2 hbar_iconpos;
    int health;
    int max_health;
    unsigned char actionmask;
    bool is_orbpos;
    bool in_hurt_anim;
} vy_t;

void vy_init(vy_t *vy);
void vy_activate(vy_t *vy);
void vy_update(vy_t *vy, float dt);
void vy_activate_hurting(vy_t *vy, float dt);
void vy_draw(vy_t *vy);


#endif