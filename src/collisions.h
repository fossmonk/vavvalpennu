#ifndef _COLLISIONS_H_
#define _COLLISIONS_H_

#include <raylib.h>
#include <obj.h>
#include <vy.h>
#include <batr.h>

typedef struct {
    bool rl;
    bool lr;
    bool tb;
    bool bt;
} col_details_2d;

bool col_check_bbox(obj_t *obj1, obj_t *obj2, col_details_2d *o_details);

#endif