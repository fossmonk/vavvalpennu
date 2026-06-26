#ifndef _KCHATH_H_
#define _KCHATH_H_

#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <hbar.h>
#include <skball.h>

// action bitmasks
#define KCH_IS_DYING     (1 << 0)
#define KCH_IS_HURTING_S (1 << 1)

#define kch_is_dying(kch)   ((kch)->actionmask & KCH_IS_DYING)
#define kch_is_hurting(kch) ((kch)->actionmask & KCH_IS_HURTING_S)

#define kch_set_die(kch)        ((kch)->actionmask |= KCH_IS_DYING)
#define kch_set_hurt_shock(kch) ((kch)->actionmask |= KCH_IS_HURTING_S)

#define kch_clr_die(kch)        ((kch)->actionmask &= ~KCH_IS_DYING)
#define kch_clr_hurt_shock(kch) ((kch)->actionmask &= ~KCH_IS_HURTING_S)

typedef struct {
    obj_t obj;
    int health;
    int max_health;
    hbar_t hbar;
    unsigned char actionmask;
    bool in_hurt_anim;

    // weapons
    skball_t skballs[MAX_SKBALLS];

    // animations
    anim_t anim_laugh;
    anim_t anim_hurt;

    // audio
    Sound hurt;
    Music laugh;
} kchath_t;


void kchath_init(kchath_t* kch);
void kchath_activate(kchath_t *kch);
void kchath_update(kchath_t *kch, float dt);
void kchath_draw(kchath_t *kch);

#endif