#ifndef _VP_BBOX_H_
#define _VP_BBOX_H_

#include <raylib.h>

typedef enum {
    RECTANGLE,
    CIRCLE,
    INVALID_BBOX_TYPE
} bbox_type;

typedef struct {
    float cx, cy, r;
} circle_t;

typedef struct {
    bbox_type type;
    union {
        Rectangle rect;
        circle_t circle;
    } bbox;
} bbox_t;

bbox_t bbox_parse(const char* bbox_fname);

#endif