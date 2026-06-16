#include <stdlib.h>
#include <raylib.h>
#include <obj.h>
#include <player.h>
#include <vy.h>
#include <aanam.h>
#include <batr.h>
#include <orb.h>
#include <collisions.h>

bool col_check_vy_batr(vy_t *vy, batr_t *b) {
    // cartesian
    obj_t *vy_obj = &vy->obj;
    obj_t *b_obj = &b->obj;
    float cartd2 = obj_cartd2(b_obj, COORDS_WORLD, vy_obj, COORDS_WORLD);
    float lim = b_obj->size.x/2;
    return (cartd2 < lim*lim);
}

bool col_check_player_orb(player_t *p, orb_t *orb) {
    Vector2 pspos = obj_w2s_pos(p->obj.pos);
    float orb_down_y = orb->obj.pos.y + orb->obj.size.y;
    float orb_c_x = orb->obj.pos.x + orb->obj.size.x/2;
    float p_up_y = pspos.y;
    float p_c_x = pspos.x + p->obj.size.x/2;
    return ((fabsf(orb_down_y - p_up_y) < 40) && (fabsf(orb_c_x - p_c_x) < 40));
}

bool col_check_player_aanam(player_t *p, aanam_t *aana) {
    // cartesian
    obj_t *p_obj = &p->obj;
    obj_t *a_obj = &aana->obj;
    float cartd2 = obj_cartd2(p_obj, COORDS_WORLD, a_obj, COORDS_WORLD);
    float lim = 20.0f;
    return (cartd2 < lim*lim && obj_cxy(&aana->obj).x > obj_cxy(&p->obj).x);
}

bool col_check_batr_orb(batr_t *b, orb_t *orb) {
    // cartesian
    obj_t *b_obj = &b->obj;
    obj_t *o_obj = &orb->obj;
    float cartd2 = obj_cartd2(b_obj, COORDS_WORLD, o_obj, COORDS_SCREEN);
    float lim = b_obj->size.x/2;
    return (cartd2 < lim*lim);
}