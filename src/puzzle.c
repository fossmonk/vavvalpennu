#include <raylib.h>
#include <vpconfig.h>
#include <puzzle.h>
#include <stdlib.h>
#include <stdio.h>
#include <rand.h>
#include <ctype.h>
#include <string.h>
#include <bbox.h>
#include <obj.h>

#define PUZZLE_COUNT (16)

typedef struct {
    char *q;
    char *a[3];
} puzzle_t;

unsigned int active_puzzle_bmap = 0U;
int curr_puzzle_idx = 0;
Font puzzlefont;
Vector2 scroll_pos;
Texture2D scroll_tex;
Rectangle scroll_write_rect;
Sound puzzle_solved;
Sound puzzle_wrong;

static int extract_word(char *line, char* word) {
    int i = 0;
    while(line[i] != ' ' && line[i] != '\0') {
        word[i] = line[i];
        ++i;
    }
    return i;
}

static void draw_rect_wrapped_text(char *text, Font font, float font_size, Rectangle rect) {
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
        offset = extract_word(p, wordbuf);
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

static puzzle_t g_puzzles[PUZZLE_COUNT] = {
    {
        .q = "It can be cruel, poetic, or blind. But when it's denied, it's violence you may find.",
        .a = {"justice", NULL, NULL}
    },
    {
        .q = "What is the beginning of eternity, the end of time and space, the beginning of every end, and the end of every race?",
        .a = {"e", "thelettere", "lettere"}
    },
    {
        .q = "The More You Take, the More You Leave Behind. What Am I?",
        .a = {"footsteps", NULL, NULL}
    },
    {
        .q = "What Comes Once in a Minute, Twice in a Moment, But Never in a Thousand Years?",
        .a = {"m", "theletterm", "letterm"}
    },
    {
        .q = "I can be cracked. I can be made. I can be told. I can be played. What am I?",
        .a = {"joke", "ajoke", NULL}
    },
    {
        .q = "The more you cut me the bigger I grow. What am I?",
        .a = {"hole", "ahole", NULL}
    },
    {
        .q = "I am light as a feather, yet the strongest man cannot hold me for much longer than a few minutes.",
        .a = {"breath", NULL, NULL}
    },
    {
        .q = "I have many eyes but cannot see. I am buried alive but never weep. From my skin, new lives will creep.",
        .a = {"potato", "apotato", NULL}
    },
    {
        .q = "I have a spine but no bones. I have leaves but no branches. I can tell you everything, yet I cannot speak a word.",
        .a = {"book", "abook", NULL}
    },
    {
        .q = "I have keys but open no locks. I have space but no room. You can enter, but you can't go outside. What am I?",
        .a = {"keyboard", "akeyboard", NULL}
    },
    {
        .q = "I am always running but I have no legs. I never look back, and no one can catch me. I heal all wounds, yet I eventually kill everyone who tracks me.",
        .a = {"time", NULL, NULL}
    },
    {
        .q = "I can run but never walk, have a bed but never sleep, have a mouth but never speak. What am I?",
        .a = {"river", "ariver", NULL}
    },
    {
        .q = "I can capture a moment forever without keeping a single memory. I can reflect your true face, but I will never look you in the eye.",
        .a = {"mirror", "amirror", NULL}
    },
    {
        .q = "I have a neck but no head, and I wear a cap but have no hair. I am always at the party, but I never dance. What am I?",
        .a = {"bottle", "abottle", NULL}
    },
    {
        .q = "I have cities but no houses, mountains but no trees, and water but no fish. What am I?",
        .a = {"map", "amap", NULL}
    },
    {
        .q = "I shave every single day, yet my beard stays exactly the same length. Who am I?",
        .a = {"barber", "abarber", NULL}
    },
};

void puzzle_init(void) {
    scroll_tex = LoadTexture(SCROLL_TEXTURE);
    scroll_pos = (Vector2) {G_W/2 - scroll_tex.width/2, G_H/2 - scroll_tex.height/2};
    scroll_write_rect = bbox_parse(SCROLL_BBOX).bbox.rect;
    puzzlefont = LoadFontEx(PUZZLE_FONT, 96, NULL, 0);
    SetTextureFilter(puzzlefont.texture, TEXTURE_FILTER_BILINEAR);
    puzzle_solved = LoadSound(SOUND_A_ACHIEVE);
    puzzle_wrong = LoadSound(SOUND_A_WRONG);
}

int puzzle_get(void) {
    if(active_puzzle_bmap == ((1 << (PUZZLE_COUNT)) - 1)) active_puzzle_bmap = 0U;

    int retries = 500;

    while(retries--) {
        int r = vp_rand_lim(0, PUZZLE_COUNT);
        if(((active_puzzle_bmap >> r) & 0x1) == 0) {
            curr_puzzle_idx = r;
            active_puzzle_bmap |= (1 << r);
            break;
        }
    }

    return curr_puzzle_idx;
}

Vector2 puzzle_get_pos(void) {
    return scroll_pos;
}

void puzzle_play_solved(void) {
    PlaySound(puzzle_solved);
}

void puzzle_play_wrong(void) {
    PlaySound(puzzle_wrong);
}

#define MAX_ANSWER_SZ (32)
bool puzzle_check(char *user_ans, int id) {
    if(user_ans == NULL)  return false;
    
    bool ret = false;
    
    // sanitize
    char ans_sbuf[MAX_ANSWER_SZ] = { 0 };
    
    for(int i = 0, j = 0; i < MAX_ANSWER_SZ; ++i) {
        if(user_ans[i] == ' ') continue;
        if(isalnum(user_ans[i])) ans_sbuf[j++] = tolower(user_ans[i]);
    }

    for(int i = 0; i < 3; ++i) {
        char *ans = g_puzzles[id].a[i];
        if(ans != NULL && (strncmp(ans_sbuf, ans, MAX_ANSWER_SZ) == 0)) {
            ret = true;
        }
    }
    return ret;
}

void puzzle_draw_q(int puzzle_id) {
    char *question = g_puzzles[puzzle_id].q;
    Rectangle r;
    r.width = scroll_write_rect.width;
    r.height = scroll_write_rect.height;
    r.x = scroll_write_rect.x + scroll_pos.x;
    r.y = scroll_write_rect.y + scroll_pos.y;
    // Draw the scroll
    DrawTexture(scroll_tex, SPREAD_VEC(scroll_pos), WHITE);
    // Draw the text
    draw_rect_wrapped_text(question, puzzlefont, 40, r);
}

void puzzle_draw_textbox(char *typebuffer) {
    int len = 0;
    char tempbuf[66] = { 0 };
    for(int i = 0; (i < 64) && typebuffer[i] != '\0'; ++i) {
        tempbuf[i] = tolower(typebuffer[i]);
        len++;
    }
    tempbuf[len] = '_';

    float width = 500, height = 40;
    DrawRectangleLines(G_W/2 - width/2, GAME_GROUND_Y, width, height, RED);
    DrawRectangle((G_W/2 - width/2)+2, GAME_GROUND_Y+2, width-4, height-4, BLACK);
    Vector2 pos = {(G_W/2 - width/2)+2, GAME_GROUND_Y+2};
    DrawTextEx(puzzlefont, tempbuf, pos, 30, 1, WHITE);
}