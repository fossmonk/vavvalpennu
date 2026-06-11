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
    bool is_active;
    anim_t *curr_anim;
    hdir_t hdir;
} obj_t;

bool obj_is_oob(obj_t *obj, coord_sys coords, Camera2D cam);
float obj_cartd2(obj_t *obj1, coord_sys cs1, obj_t *obj2, coord_sys cs2, Camera2D cam);

/// SOME STATIC INLINES
// Object Center XY
static inline Vector2 obj_cxy(obj_t *obj) {
    float w = fabsf(obj->curr_anim->curr_frame.width);
    float h = obj->curr_anim->curr_frame.height;
    return (Vector2){(obj->pos.x + w/2) , (obj->pos.y + h/2)};
} 

// World X greater than Screen Limit
static inline bool obj_wx_gt_s(obj_t *obj, float lim, Camera2D cam) {
    return GetWorldToScreen2D(obj->pos, cam).x > lim;
}

// World X less than Screen Limit
static inline bool obj_wx_lt_s(obj_t *obj, float lim, Camera2D cam) {
    return GetWorldToScreen2D(obj->pos, cam).x < lim;
}

// World Y greater than Screen Limit
static inline bool obj_wy_gt_s(obj_t *obj, float lim, Camera2D cam) {
    return GetWorldToScreen2D(obj->pos, cam).y > lim;
}

// World Y less than Screen Limit
static inline bool obj_wy_lt_s(obj_t *obj, float lim, Camera2D cam) {
    return GetWorldToScreen2D(obj->pos, cam).y < lim;
}

#endif