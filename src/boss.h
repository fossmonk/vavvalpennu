#include <raylib.h>
#include <vy.h>
#include <epechi.h>
#include <kchath.h>
#include <levels.h>

#ifndef _BOSS_H_
#define _BOSS_H_

typedef enum {
    KCHATHAN,
    EPECHI,
    ARUKOLA,
    VADAYAKSHI
} BOSS_TYPE;

typedef struct {
    kchath_t kch;
    vy_t vy;
    epechi_t ep;
} bosses;

void boss_init_bosses(void);
bosses* boss_get_bosses(void);
void boss_set(vplevel l);
BOSS_TYPE boss_get(void);
void boss_activate(void);
bool boss_is_active(void);
void boss_update(float dt);
bool boss_is_dead(void);
void boss_draw(void);

#endif