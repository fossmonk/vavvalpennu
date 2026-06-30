#ifndef _VP_BBOX_H_
#define _VP_BBOX_H_

#include <raylib.h>

#define MAX_POLY_N (16)

typedef enum {
    RECTANGLE,
    CIRCLE,
    POLYGON,
    INVALID_BBOX_TYPE
} bbox_type;

typedef struct {
    float cx, cy, r;
} circle_t;

typedef struct {
    int n;
    Vector2 v[MAX_POLY_N];
} polygon_t;

typedef struct {
    bbox_type type;
    union {
        Rectangle rect;
        circle_t circle;
        polygon_t poly;
    } bbox;
} bbox_t;

bbox_t bbox_parse(const char* bbox_fname);
void bbox_draw(bbox_t bbox, Vector2 objpos, Color color);

#endif