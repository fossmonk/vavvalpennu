#ifndef _COLLISIONS_H_
#define _COLLISIONS_H_

#include <raylib.h>
#include <obj.h>
#include <vy.h>
#include <batr.h>

bool col_check_vy_batr(vy_t *vy, batr_t *b);
bool col_check_player_orb(player_t *p, orb_t *orb);
bool col_check_player_aanam(player_t *p, aanam_t *aana);
bool col_check_batr_orb(batr_t *b, orb_t *orb);

#endif