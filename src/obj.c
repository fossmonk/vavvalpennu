#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <obj.h>
#include <bbox.h>

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

bool obj_is_oob(obj_t *obj) {
    Vector2 screenpos = (obj->cs == COORDS_WORLD) ? obj_w2s_pos(obj->pos) : obj->pos;

    bool x_l = screenpos.x + obj->size.x < 0;
    bool x_h = screenpos.x > G_W;
    bool y_l = screenpos.y + obj->size.y < 0;
    bool y_h = screenpos.y > G_H;
    return (x_l || x_h || y_l || y_h);
}

Rectangle obj_get_bbox_rect(obj_t *obj) {
    Rectangle r;
    bbox_t bbox = obj->curr_anim->asset->bbox;
    switch (bbox.type)
    {
        case CIRCLE:
            r.x = bbox.bbox.circle.cx - bbox.bbox.circle.r;
            r.y = bbox.bbox.circle.cy - bbox.bbox.circle.r;
            r.width = 2*bbox.bbox.circle.r;
            r.height = 2*bbox.bbox.circle.r;
            break;
        case RECTANGLE:
            r = bbox.bbox.rect;
            break;
        case POLYGON:
        default:
            /* TODO */
            r = (Rectangle){0,0,0,0};
            break;
    }
    return r;
}