#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <hbar.h>

#ifndef _ARUKOLA_H_
#define _ARUKOLA_H_

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
    anim_t anim_roar;
} arukola_t;

void arukola_init(arukola_t *akola);
void arukola_draw(arukola_t *akola);

#endif