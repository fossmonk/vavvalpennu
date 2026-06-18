#include <stdlib.h>
#include <raylib.h>
#include <obj.h>
#include <player.h>
#include <vy.h>
#include <aanam.h>
#include <batr.h>
#include <orb.h>
#include <karikku.h>
#include <collisions.h>

#define IN_RANGE(x, l, r) ((x) >= (l) && (x) >= (r))

bool col_check_vy_batr(vy_t *vy, batr_t *b) {
    // cartesian
    obj_t *vy_obj = &vy->obj;
    obj_t *b_obj = &b->obj;
    float cartd2 = obj_cartd2(b_obj, COORDS_WORLD, vy_obj, COORDS_WORLD);
    float lim = b_obj->size.x/2;
    return (cartd2 < lim*lim);
}

bool col_check_vy_orb(vy_t *vy, orb_t *orb) {
    // cartesian
    obj_t *vy_obj = &vy->obj;
    obj_t *orb_obj = &orb->obj;
    float cartd2 = obj_cartd2(orb_obj, COORDS_SCREEN, vy_obj, COORDS_WORLD);
    float lim = orb_obj->size.x/2;
    return (cartd2 < lim*lim);
}

bool col_check_player_orb(player_t *p, orb_t *orb) {
    Vector2 pspos = obj_w2s_pos(p->obj.pos);
    float orb_down_y = orb->obj.pos.y + orb->obj.size.y;
    float orb_c_x = orb->obj.pos.x + orb->obj.size.x/2;
    float p_up_y = pspos.y;
    float p_c_x = pspos.x + p->obj.size.x/2;
    return ((fabsf(orb_down_y - p_up_y) < 30) && (fabsf(orb_c_x - p_c_x) < 40));
}

bool col_check_player_karikku(player_t *p, karikku_t *k) {
    Vector2 kc = obj_w2s_pos(obj_cxy(&k->obj));
    Vector2 pc = obj_w2s_pos(obj_cxy(&p->obj));
    return ((fabsf(kc.x - pc.x) <= 5) && (fabsf(pc.y - kc.y) <= 70));
}

bool col_check_player_aanam(player_t *p, aanam_t *aana) {
    obj_t *p_obj = &p->obj;
    obj_t *a_obj = &aana->obj;
    float lim = 1.0f;
    float p_rbx = obj_cxy(p_obj).x + PLAYER_HITBOX_RB;
    float a_lbx = a_obj->pos.x;
    float p_dby = p_obj->pos.y + p_obj->size.y;
    float a_uby = obj_cxy(a_obj).y;

    return ((a_lbx > p_rbx) && (a_lbx - p_rbx) < lim && (p_dby > a_uby));
}

bool col_check_batr_orb(batr_t *b, orb_t *orb) {
    // cartesian
    obj_t *b_obj = &b->obj;
    obj_t *o_obj = &orb->obj;
    float cartd2 = obj_cartd2(b_obj, COORDS_WORLD, o_obj, COORDS_SCREEN);
    float lim = b_obj->size.x/2;
    return (cartd2 < lim*lim);
}