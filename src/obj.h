#include <raylib.h>
#include <anim.h>

#ifndef _OBJ_H_
#define _OBJ_H_

#define COORDS_SCREEN (0)
#define COORDS_WORLD  (1)

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

bool obj_is_oob(obj_t *obj, int coords, Camera2D cam);

#endif