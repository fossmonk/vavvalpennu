#include <raylib.h>
#include <vpconfig.h>
#include <puzzle.h>
#include <stdlib.h>
#include <rand.h>
#include <ctype.h>
#include <string.h>
#include <bbox.h>
#include <textengine.h>
#include <obj.h>

#define PUZZLE_COUNT (6)

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
    }
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
    if(active_puzzle_bmap == ((1 << PUZZLE_COUNT) - 1)) active_puzzle_bmap = 0U;

    int retries = 100;

    while(retries--) {
        int r = vp_rand_lim(0, PUZZLE_COUNT);
        if(((active_puzzle_bmap >> r) & 0x1) == 0) {
            curr_puzzle_idx = r;
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

#define MAX_ANSWER_SZ (16)
bool puzzle_check(char *user_ans, int id) {
    if(user_ans == NULL)  return false;

    bool ret = false;

    // sanitize
    char ans_sbuf[MAX_ANSWER_SZ] = { 0 };

    for(int i = 0; i < MAX_ANSWER_SZ; ++i) {
        if(user_ans[i] == '\n' || user_ans[i] == '0' || user_ans[i] == '\r')break;
        if(isspace(user_ans[i])) continue;
        if(isalnum(user_ans[i])) ans_sbuf[i] = tolower(user_ans[i]);
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
    DrawTexture(scroll_tex, scroll_pos.x, scroll_pos.y, WHITE);
    // Draw the text
    te_draw_inside_rect(question, puzzlefont, 40, r);
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