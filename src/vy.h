#include <raylib.h>
#include <obj.h>
#include <hbar.h>

#ifndef _VY_H_
#define _VY_H_

typedef struct {
    obj_t obj;
    int health;
    int max_health;
    hbar_t hbar;
    bool is_dying;
    bool is_jumping;
    bool is_orbpos;

    // animations
    anim_t anim_vy_rise;
} vy_t;

void vy_init(vy_t *vy);

#endif