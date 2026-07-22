#ifndef _COLLISIONS_H_
#define _COLLISIONS_H_

#include <raylib.h>
#include <obj.h>
#include <vy.h>
#include <batr.h>

bool col_check_bbox(obj_t *obj1, obj_t *obj2, Vector2 *overlap);

#endif