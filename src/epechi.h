#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <hbar.h>

#ifndef _EPECHI_H_
#define _EPECHI_H_

typedef struct {
    obj_t obj;
    int health;
    int max_health;
    hbar_t hbar;
    Texture2D hbar_icon;
    Vector2 hbar_iconpos;
    bool is_dying;
    bool is_rolling;

    // animations
    anim_t anim_roll;
} epechi_t;

void epechi_init(epechi_t *ep);

static inline Vector2 epechi_get_initial_spos(void) {
    return (Vector2){G_W-300, GAME_GROUND_Y-190};
}

#endif