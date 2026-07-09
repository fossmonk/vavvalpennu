#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>

Camera2D *gcam2d = NULL;

void obj_global_set_cam2d(Camera2D *camref) {
    gcam2d = camref;
}

Vector2 obj_w2s_pos(Vector2 pos) {
    return GetWorldToScreen2D(pos, *gcam2d);
}

Vector2 obj_s2w_pos(Vector2 pos) {
    return GetScreenToWorld2D(pos, *gcam2d);
}

bool obj_is_oob(obj_t *obj, coord_sys coords) {
    Vector2 screenpos = (coords == COORDS_WORLD) ? obj_w2s_pos(obj->pos) : obj->pos;

    bool x_l = screenpos.x + obj->size.x < 0;
    bool x_h = screenpos.x > G_W;
    bool y_l = screenpos.y + obj->size.y < 0;
    bool y_h = screenpos.y > G_H;
    return (x_l || x_h || y_l || y_h);
}

