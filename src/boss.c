#include <raylib.h>
#include <vpconfig.h>
#include <boss.h>

#include <vy.h>
#include <epechi.h>

static BOSS_TYPE boss;
static bosses gbosses;

void boss_init_bosses(void) {
    // VADAYAKSHI
    vy_init(&gbosses.vy);
}

bosses* boss_get_bosses(void) {
    return &gbosses;
}

void boss_set(BOSS_TYPE b) {
    boss = b;
}

BOSS_TYPE boss_get(void) {
    return boss;
}

void boss_activate(void) {
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
        /* code */
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
        /* code */
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
