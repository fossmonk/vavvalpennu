#include <raylib.h>
#include <vpconfig.h>
#include <boss.h>
#include <levels.h>

#include <vy.h>
#include <epechi.h>
#include <kchath.h>

static BOSS_TYPE boss;
static bosses gbosses;

void boss_init_bosses(void) {
    // VADAYAKSHI
    vy_init(&gbosses.vy);
    // KUTTICHATHAN
    kchath_init(&gbosses.kch);
    // ARUKOLA
    // EPECHI
}

bosses* boss_get_bosses(void) {
    return &gbosses;
}

void boss_set(vplevel l) {
    // TODO: set this correctly when all bosses are configured.
    #if 0
    static const BOSS_TYPE level2boss[] = {
        [VP_L0] = KCHATHAN,
        [VP_L1] = EPECHI,
        [VP_L2] = ARUKOLA,
        [VP_L3] = VADAYAKSHI,
    };

    boss = level2boss[l];
    #else
    boss = KCHATHAN;
    #endif
}

BOSS_TYPE boss_get(void) {
    return boss;
}

void boss_activate(void) {
    switch (boss)
    {
    case KCHATHAN:
        kchath_activate(&gbosses.kch);
        break;
    case EPECHI:
        /* code */
        break;
    case ARUKOLA:
        /* code */
        break;
    case VADAYAKSHI:
        vy_activate(&gbosses.vy);
        break;
    
    default:
        break;
    }
}

bool boss_is_active(void) {
    bool ret = false;
    switch (boss)
    {
    case KCHATHAN:
        /* code */
        break;
    case EPECHI:
        /* code */
        break;
    case ARUKOLA:
        /* code */
        break;
    case VADAYAKSHI:
        ret = gbosses.vy.obj.is_active;
        break;
    
    default:
        break;
    }

    return ret;
}

void boss_update(float dt) {
    switch (boss)
    {
    case KCHATHAN:
        kchath_update(&gbosses.kch, dt);
        break;
    case EPECHI:
        /* code */
        break;
    case ARUKOLA:
        /* code */
        break;
    case VADAYAKSHI:
        vy_update(&gbosses.vy, dt);
        break;
    
    default:
        break;
    }
}

bool boss_is_dead(void) {
    bool ret = false;
    switch (boss)
    {
    case KCHATHAN:
        ret = (gbosses.kch.health == 0);
        break;
    case EPECHI:
        /* code */
        break;
    case ARUKOLA:
        /* code */
        break;
    case VADAYAKSHI:
        ret = (gbosses.vy.health == 0);
        break;
    
    default:
        break;
    }
    return ret;
}

void boss_draw(void) {
    switch (boss)
    {
    case KCHATHAN:
        /* code */
        break;
    case EPECHI:
        /* code */
        break;
    case ARUKOLA:
        /* code */
        break;
    case VADAYAKSHI:
        vy_draw(&gbosses.vy);
        break;
    
    default:
        break;
    }
}
