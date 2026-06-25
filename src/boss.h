#include <raylib.h>
#include <vy.h>
#include <epechi.h>

#ifndef _BOSS_H_
#define _BOSS_H_

typedef enum {
    KCHATHAN,
    EPECHI,
    ARUKOLA,
    VADAYAKSHI
} BOSS_TYPE;

typedef struct {
    vy_t vy;
    epechi_t ep;
} bosses;

void boss_init_bosses(void);
bosses* boss_get_bosses(void);
void boss_set(BOSS_TYPE b);
BOSS_TYPE boss_get(void);
void boss_activate(void);
bool boss_is_active(void);
void boss_update(float dt);
bool boss_is_dead(void);
void boss_draw(void);

#endif