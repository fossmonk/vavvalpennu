#include <raylib.h>
#include <bbox.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

bbox_t bbox_parse(const char* bbox_fname) {
    bbox_t b;
    char *bbox_text = LoadFileText(bbox_fname);

    char* secondline = NULL;
    int linebr_idx = 0;
    while(bbox_text[linebr_idx] != '\n')linebr_idx++;

    secondline = &bbox_text[linebr_idx+1];

    char *fstr_type = "bbox_type=%s";
    char btype[15] = {0};
    sscanf(bbox_text, fstr_type, btype);
    if(strncmp("rectangle", btype, 14) == 0) {
        char *fstr_rect = "bbox_params=[%d,%d,%d,%d]";
        int x,y,w,h;
        sscanf(secondline, fstr_rect, &x, &y, &w, &h);
        b.type = RECTANGLE;
        b.bbox.rect = (Rectangle){x, y, w, h};
    } else if(strncmp("circle", btype, 14) == 0) {
        char *fstr_circle = "bbox_params=[%d,%d,%d]";
        int cx, cy, r;
        sscanf(secondline, fstr_circle, &cx, &cy, &r);
        b.type = CIRCLE;
        b.bbox.circle = (circle_t){cx, cy, r};
    } else {
        b.type = INVALID_BBOX_TYPE;
    }
    UnloadFileText(bbox_text);
    return b;
}