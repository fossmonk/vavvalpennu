#include <raylib.h>
#include <vy.h>
#include <epechi.h>
#include <kchath.h>
#include <arukola.h>
#include <levels.h>

#ifndef _BOSS_H_
#define _BOSS_H_

typedef enum {
    KCHATHAN,
    EPECHI,
    ARUKOLA,
    VADAYAKSHI,
    NONE
} boss_type;

typedef struct {
    kchath_t kch;
    arukola_t akola;
    epechi_t ep;
    vy_t vy;
} bosses_t;

void boss_init_bosses(bosses_t *bosses);
boss_type boss_get_for_level(vplevel l);
void boss_activate(boss_type boss);
bool boss_is_active(boss_type boss);
void boss_update(boss_type boss, float dt);
bool boss_is_dead(boss_type boss);
void boss_handle_death(boss_type boss);
void boss_draw(boss_type boss);

#endif