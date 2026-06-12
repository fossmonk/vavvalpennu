#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>

Camera2D *gcam2d = NULL;

void obj_global_set_cam2d(Camera2D *camref) {
    gcam2d = camref;
}

bool obj_is_oob(obj_t *obj, coord_sys coords) {
    Vector2 screenpos = (coords == COORDS_WORLD) ? 
        GetWorldToScreen2D(obj->pos, *gcam2d) : obj->pos;

    bool x_l = screenpos.x + obj->size.x < 0;
    bool x_h = screenpos.x > G_W;
    bool y_l = screenpos.y + obj->size.y < 0;
    bool y_h = screenpos.y > G_H;
    return (x_l || x_h || y_l || y_h);
}

float obj_cartd2(obj_t *obj1, coord_sys cs1, obj_t *obj2, coord_sys cs2) {
    Vector2 obj1_pos = (cs1 == COORDS_WORLD) ? GetWorldToScreen2D(obj1->pos, *gcam2d) : obj1->pos;
    Vector2 obj2_pos = (cs2 == COORDS_WORLD) ? GetWorldToScreen2D(obj2->pos, *gcam2d) : obj2->pos;
    
    float dcx = (obj1_pos.x + obj1->size.x/2) - (obj2_pos.x + obj2->size.x/2);
    float dcy = (obj1_pos.y + obj1->size.y/2) - (obj2_pos.y + obj2->size.y/2);
    return dcx*dcx + dcy*dcy;
}