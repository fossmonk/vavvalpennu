#include <raylib.h>
#include <textengine.h>
#include <string.h>
#include <stdio.h>

static int te_extract_word(char *line, char* word) {
    int i = 0;
    while(line[i] != ' ' && line[i] != '\0') {
        word[i] = line[i];
        ++i;
    }
    return i;
}

void te_draw_inside_rect(char *text, Font font, float font_size, Rectangle rect) {
    // shrink rectangle
    float margin = 2.0f;
    rect.x += margin;
    rect.y += margin;
    rect.width -= 2*margin;
    rect.height -= 2*margin;

    float x_ptr = 0.0f, y_ptr = 0.0f;
    char *p = text;
    char wordbuf[32] = { 0 };

    // handle special cases
    if(p == NULL) return;

    int offset = 0;
    Vector2 pos;
    while(*p != '\0') {
        offset = te_extract_word(p, wordbuf);
        if(p[offset] != '\0') {
            wordbuf[offset] = ' ';
            wordbuf[offset+1] = '\0';
        }
        if(p[offset] == ' ')offset++;
        
        Vector2 w_sz = MeasureTextEx(font, wordbuf, font_size, 1);
        
        if(w_sz.x <= (rect.width - x_ptr)) {
            pos = (Vector2){x_ptr + rect.x, y_ptr + rect.y};
            DrawTextEx(font, wordbuf, pos, font_size, 1, WHITE);
            x_ptr += w_sz.x;
        } else {
            y_ptr += (w_sz.y + 1.0f);
            x_ptr = 0.0f;
            if(w_sz.x <= rect.width) {
                pos = (Vector2){x_ptr + rect.x, y_ptr + rect.y};
                DrawTextEx(font, wordbuf, pos, font_size, 1, WHITE);
                x_ptr += w_sz.x;
            } else {
                printf("[VP WARNING] word %s is OOB !!\n", wordbuf);
            }
        }

        if(x_ptr >= rect.width) {
            y_ptr += (w_sz.y + 1.0f);
            x_ptr = 0.0f;
        } 
            
        for(int z = 0; z < 32; ++z)wordbuf[z] = '\0';
        p += offset;
    }
}