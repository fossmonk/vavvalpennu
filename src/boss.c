#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <boss.h>
#include <levels.h>

#include <vy.h>
#include <epechi.h>
#include <kchath.h>
#include <arukola.h>

bosses_t *gref_bosses = NULL;

boss_type boss_level_map[] = {
    [VP_L0] = KCHATHAN,
    [VP_L1] = VADAYAKSHI,
    [VP_L2] = VADAYAKSHI,
    [VP_L3] = VADAYAKSHI,
};

void boss_init_bosses(bosses_t *bosses) {
    // VADAYAKSHI
    vy_init(&bosses->vy);
    // KUTTICHATHAN
    kchath_init(&bosses->kch);
    // ARUKOLA
    // EPECHI

    // set the global reference
    gref_bosses = bosses;
}

boss_type boss_get_for_level(vplevel l) {
    return boss_level_map[l];
}

void boss_activate(boss_type boss) {
    switch (boss)
    {
    case KCHATHAN:
        kchath_activate(&gref_bosses->kch);
        break;
    case EPECHI:
        /* code */
        break;
    case ARUKOLA:
        /* code */
        break;
    case VADAYAKSHI:
        vy_activate(&gref_bosses->vy);
        break;
    
    default:
        break;
    }
}

bool boss_is_active(boss_type boss) {
    bool ret = false;
    switch (boss)
    {
    case KCHATHAN:
        ret = gref_bosses->kch.obj.is_active;
        break;
    case EPECHI:
        ret = gref_bosses->ep.obj.is_active;
        break;
    case ARUKOLA:
        ret = gref_bosses->akola.obj.is_active;
        break;
    case VADAYAKSHI:
        ret = gref_bosses->vy.obj.is_active;
        break;
    
    default:
        break;
    }

    return ret;
}

void boss_update(boss_type boss, float dt) {
    switch (boss)
    {
    case KCHATHAN:
        kchath_update(&gref_bosses->kch, dt);
        break;
    case EPECHI:
        /* code */
        break;
    case ARUKOLA:
        /* code */
        break;
    case VADAYAKSHI:
        vy_update(&gref_bosses->vy, dt);
        break;
    
    default:
        break;
    }
}

bool boss_is_dead(boss_type boss) {
    bool ret = false;
    switch (boss)
    {
    case KCHATHAN:
        ret = (gref_bosses->kch.obj.is_active == false);
        break;
    case EPECHI:
        /* code */
        break;
    case ARUKOLA:
        /* code */
        break;
    case VADAYAKSHI:
        ret = (gref_bosses->vy.obj.is_active == 0);
        break;
    
    default:
        break;
    }
    return ret;
}

void boss_handle_death(boss_type boss) {
    switch (boss)
    {
    case KCHATHAN:
        kchath_handle_death(&gref_bosses->kch);
        break;
    case EPECHI:
        /* code */
        break;
    case ARUKOLA:
        /* code */
        break;
    case VADAYAKSHI:
        vy_handle_death(&gref_bosses->vy);
        break;
    
    default:
        break;
    }
}

void boss_draw(boss_type boss) {
    switch (boss)
    {
    case KCHATHAN:
        kchath_draw(&gref_bosses->kch);
        break;
    case EPECHI:
        /* code */
        break;
    case ARUKOLA:
        /* code */
        break;
    case VADAYAKSHI:
        vy_draw(&gref_bosses->vy);
        break;
    
    default:
        break;
    }
}
