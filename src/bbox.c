#include <raylib.h>
#include <bbox.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

static void bbox_parse_poly_v(char * poly_params, polygon_t *p) {
    if(p->n > MAX_POLY_N) return;

    int idx = 0;
    while(poly_params[idx] != '[')idx++;
    char *ptr = &poly_params[idx+1];

    for(int i = 0; i < p->n; i++) {
        int x, y, count;
        sscanf(ptr, "%d,%d,%n", &x, &y, &count);
        p->v[i].x = x;
        p->v[i].y = y;
        ptr += count;
    }
}

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
    } else if(strncmp("polygon", btype, 14) == 0) {
        char *fstr_polyn = "bbox_poly_n=%d";
        int n;
        sscanf(secondline, fstr_polyn, &n);
        b.type = POLYGON;
        b.bbox.poly.n = n;
        char *thirdline = NULL;
        int idx = 0;
        while(secondline[idx] != '\n')idx++;
        thirdline = &secondline[idx+1];
        bbox_parse_poly_v(thirdline, &b.bbox.poly);
    } else {
        b.type = INVALID_BBOX_TYPE;
    }
    UnloadFileText(bbox_text);
    return b;
}

// for debug purposes
void bbox_draw(bbox_t bbox, Vector2 objpos, Color color) {
    float cx, cy, r, x, y, w, h;
    switch (bbox.type)
    {
    case CIRCLE:
        cx = bbox.bbox.circle.cx + objpos.x;
        cy = bbox.bbox.circle.cy + objpos.y;
        r = bbox.bbox.circle.r;
        DrawCircleLines(cx, cy, r, RED);
        break;
    case RECTANGLE:
        x = bbox.bbox.rect.x + objpos.x;
        y = bbox.bbox.rect.y + objpos.y;
        w = bbox.bbox.rect.width;
        h = bbox.bbox.rect.height;
        DrawRectangleLines(x, y, w, h, color);
        break;
    
    default:
        break;
    }
}