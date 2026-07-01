#include <raylib.h>
#include <anim.h>

#ifndef _OBJ_H_
#define _OBJ_H_

typedef enum {
    COORDS_SCREEN,
    COORDS_WORLD
} coord_sys;

typedef enum {
    LEFT,
    RIGHT
} hdir_t;

typedef struct {
    Vector2 pos;
    Vector2 vel;
    Vector2 size;
    anim_t *curr_anim;
    hdir_t hdir;
    bool is_active;
} obj_t;

void obj_global_set_cam2d(Camera2D *camref);
bool obj_is_oob(obj_t *obj, coord_sys coords);
float obj_cartd2(obj_t *obj1, coord_sys cs1, obj_t *obj2, coord_sys cs2);
Vector2 obj_w2s_pos(Vector2 pos);
Vector2 obj_s2w_pos(Vector2 pos);

/// SOME STATIC INLINES
// Object Center XY
static inline Vector2 obj_cxy(obj_t *obj) {
    float w = fabsf(obj->curr_anim->curr_frame.width);
    float h = obj->curr_anim->curr_frame.height;
    return (Vector2){(obj->pos.x + w/2) , (obj->pos.y + h/2)};
} 


#endif