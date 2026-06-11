#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>

bool obj_is_oob(obj_t *obj, int coords, Camera2D cam) {
    Vector2 screenpos = obj->pos;
    if(coords == COORDS_WORLD) {
        screenpos = GetWorldToScreen2D(obj->pos, cam);
    }
    if((screenpos.y < 0) || (screenpos.y > G_H) || (screenpos.x > G_W) || (screenpos.x < 0)) return true;
    return false;
}