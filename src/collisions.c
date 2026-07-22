#include <stdlib.h>
#include <stdio.h>
#include <raylib.h>
#include <raymath.h>
#include <obj.h>
#include <player.h>
#include <vy.h>
#include <aanam.h>
#include <batr.h>
#include <orb.h>
#include <karikku.h>
#include <collisions.h>
#include <bbox.h>

#define IN_RANGE(x, l, r) ((x) >= (l) && (x) >= (r))
#define CIRCLE_BBOX(obj) ((obj)->curr_anim->asset->bbox.bbox.circle)
#define RECT_BBOX(obj) ((obj)->curr_anim->asset->bbox.bbox.rect)

static bool col_check_c2c(obj_t *obj1, obj_t *obj2, Vector2* overlap) {
    // radial check
    float radius_sum = CIRCLE_BBOX(obj1).r + CIRCLE_BBOX(obj2).r;
    Vector2 c1 = (Vector2){
                    CIRCLE_BBOX(obj1).cx + obj1->pos.x,
                    CIRCLE_BBOX(obj1).cy + obj1->pos.y
                };
    Vector2 c2 = (Vector2){
                    CIRCLE_BBOX(obj2).cx + obj2->pos.x,
                    CIRCLE_BBOX(obj2).cy + obj2->pos.y
                };

    c1 = (obj1->cs == COORDS_WORLD) ? obj_w2s_pos(c1) : c1;
    c2 = (obj2->cs == COORDS_WORLD) ? obj_w2s_pos(c2) : c2;
    
    float dsqr = (c1.x-c2.x)*(c1.x-c2.x) + (c1.y-c2.y)*(c1.y-c2.y);

    bool is_colliding = dsqr <= (radius_sum * radius_sum);

    if(overlap != NULL && is_colliding) {
        // fill overlap details
        // overlap x = (w1+w2)/2 - |cx1 - cx2|
        // overlap y = (h1+h2)/2 - |cy1 - cy2|
        // In case of circles, (w1+w2)/2 = (h1+h2)/2 = r1+r2
        overlap->x = radius_sum - fabsf(c1.x - c2.x);
        overlap->y = radius_sum - fabsf(c1.y - c2.y);
    }

    return is_colliding;
}

static bool col_check_r2r(obj_t *obj1, obj_t *obj2, Vector2* overlap) {
    // convert all screencords
    Vector2 obj1_spos = (obj1->cs == COORDS_WORLD) ? obj_w2s_pos(obj1->pos) : obj1->pos;
    Vector2 obj2_spos = (obj2->cs == COORDS_WORLD) ? obj_w2s_pos(obj2->pos) : obj2->pos;

    // X
    float x1min = RECT_BBOX(obj1).x + obj1_spos.x;
    float x1max = x1min + RECT_BBOX(obj1).width;
    float x2min = RECT_BBOX(obj2).x + obj2_spos.x;
    float x2max = x2min + RECT_BBOX(obj2).width;
    // Y
    float y1min = RECT_BBOX(obj1).y + obj1_spos.y;
    float y1max = y1min + RECT_BBOX(obj1).height;
    float y2min = RECT_BBOX(obj2).y + obj2_spos.y;
    float y2max = y2min + RECT_BBOX(obj2).height;

    bool is_colliding = (x1max > x2min) && (x1min < x2max) &&
                         (y1max > y2min) && (y1min < y2max);

    if(overlap != NULL && is_colliding) {
        // fill overlap details
        // overlap x = (w1+w2)/2 - |cx1 - cx2|
        // overlap y = (h1+h2)/2 - |cy1 - cy2|
        float w1 = RECT_BBOX(obj1).width;
        float w2 = RECT_BBOX(obj2).width;
        float h1 = RECT_BBOX(obj1).height;
        float h2 = RECT_BBOX(obj2).height;

        Vector2 c1 = (Vector2){x1min + 0.5*w1, y1min + 0.5*h1};
        Vector2 c2 = (Vector2){x2min + 0.5*w2, y2min + 0.5*h2};

        overlap->x = (0.5f * (w1+w2)) - fabsf(c1.x - c2.x);
        overlap->y = (0.5f * (h1+h2)) - fabsf(c1.y - c2.y);
    }

    return is_colliding;
}

static bool col_check_c2r(obj_t *c_obj, obj_t *r_obj, Vector2* overlap) {
    // convert all screencords
    Vector2 c_obj_spos = (c_obj->cs == COORDS_WORLD) ? obj_w2s_pos(c_obj->pos) : c_obj->pos;
    Vector2 r_obj_spos = (r_obj->cs == COORDS_WORLD) ? obj_w2s_pos(r_obj->pos) : r_obj->pos;
    float c_x = CIRCLE_BBOX(c_obj).cx + c_obj_spos.x;
    float c_y = CIRCLE_BBOX(c_obj).cy + c_obj_spos.y;
    float r_x = RECT_BBOX(r_obj).x + r_obj_spos.x;
    float r_y = RECT_BBOX(r_obj).y + r_obj_spos.y;
    
    float test_x = c_x;
    float test_y = c_y;
    /*
    src: https://www.jeffreythompson.org/collision-detection/
    If the circle is to the LEFT of the square, check against the LEFT edge.
    If the circle is to the RIGHT of the square, check against the RIGHT edge.
    If the circle is to the BELOW the square, check against the BOTTOM edge.
    If the circle is ABOVE the square, check against the TOP edge.
    */
    if(c_x < r_x) {
        test_x = r_x;
    } else if(c_x > (r_x + RECT_BBOX(r_obj).width)) {
        test_x = (r_x + RECT_BBOX(r_obj).width);
    }

    if(c_y < r_y) {
        test_y = r_y;
    } else if(c_y > (r_y + RECT_BBOX(r_obj).height)) {
        test_y = (r_y + RECT_BBOX(r_obj).height);
    } 

    float dist_x = c_x - test_x;
    float dist_y = c_y - test_y;
    float dsqr = (dist_x * dist_x) + (dist_y * dist_y);
    float rsqr = CIRCLE_BBOX(c_obj).r * CIRCLE_BBOX(c_obj).r;

    bool is_colliding = dsqr <= rsqr;

    if(overlap != NULL && is_colliding) {
        // fill overlap details
        // overlap x = (w1+w2)/2 - |cx1 - cx2|
        // overlap y = (h1+h2)/2 - |cy1 - cy2|
        float w1 = 2 * CIRCLE_BBOX(c_obj).r;
        float h1 = w1;
        float w2 = RECT_BBOX(r_obj).width;
        float h2 = RECT_BBOX(r_obj).height;
        float rcx = r_x + w2*0.5;
        float rcy = r_y + h2*0.5;

        overlap->x = 0.5*(w1+w2) - fabsf(c_x - rcx);
        overlap->y = 0.5*(h1+h2) - fabsf(c_y - rcy);
    }
    
    return is_colliding;
}

bool col_check_bbox(obj_t *obj1, obj_t *obj2, Vector2* overlap) {
    bool colliding = false;
    
    bbox_type bt1 = obj1->curr_anim->asset->bbox.type;
    bbox_type bt2 = obj2->curr_anim->asset->bbox.type;

    if(bt1 >= INVALID_BBOX_TYPE || bt2 >= INVALID_BBOX_TYPE) {
        return false;
    }

    if(bt1 == CIRCLE && bt2 == CIRCLE) {
        colliding = col_check_c2c(obj1, obj2, overlap);
    } else if(bt1 == RECTANGLE && bt2 == RECTANGLE) {
        colliding = col_check_r2r(obj1, obj2, overlap);
    } else {
        obj_t *c_obj;
        obj_t *r_obj;

        if(bt1 == CIRCLE) {
            c_obj = obj1;
            r_obj = obj2;
        } else {
            c_obj = obj2;
            r_obj = obj1;
        }
        colliding = col_check_c2r(c_obj, r_obj, overlap);
    }
    
    return colliding;
}
