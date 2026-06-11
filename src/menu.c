#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <menu.h>

#define MENU_TOP_Y (256)
#define CONTROLS_TOP_Y (300)
#define MENU_FONT_SIZE (35)
#define MENU_SPACING (0.75)

typedef enum {
    MENU_MAIN,
    MENU_CONTROLS
} menu_state;

static menu_state mstate = MENU_MAIN;
static char *menu_start = "START GAME";
static char *menu_pause = "RESUME GAME";
static char *menu_controls = "CONTROLS";
static char *menu_quit = "QUIT GAME";
static char *menu_back = "GO BACK";

static char* controls_text[] = {
    "          CONTROLS         ",
    "===========================",
    "MOVE LEFT                 A",
    "MOVE RIGHT                D",
    "JUMP                  SPACE",
    "THROW BATARANG          RMB",
    "HEAVY ATTACK            LMB",
    NULL
};

static Font menu_font;
static Rectangle start_bb = {0, 0, 0, 0};
static Rectangle pause_bb = {0, 0, 0, 0};
static Rectangle controls_bb = {0, 0, 0, 0};
static Rectangle quit_bb = {0, 0, 0, 0};
static Rectangle back_bb = {0, 0, 0, 0};

Vector2 get_scaled_mouse_pos(void) {
    Vector2 mouseScalePos = GetMousePosition();
    mouseScalePos.x = (mouseScalePos.x / GetScreenWidth()) * G_W;
    mouseScalePos.y = (mouseScalePos.y / GetScreenHeight()) * G_H;
    return mouseScalePos;
}

bool point_in_rect(Vector2 p, Rectangle r) {
    float xl = r.x;
    float xh = r.x + r.width;
    float yl = r.y;
    float yh = r.y + r.height;

    if(p.x > xl && p.x < xh && p.y > yl && p.y < yh) return true;
    return false;
}

menu_action menu_get_action(void) {
    menu_action ma = MENU_NO_ACTION;
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mouse_pos = get_scaled_mouse_pos();
        if(point_in_rect(mouse_pos, start_bb)) ma = MENU_CLICK_START;
        else if(point_in_rect(mouse_pos, controls_bb)) ma = MENU_CLICK_CONTROLS;
        else if(point_in_rect(mouse_pos, quit_bb)) ma = MENU_CLICK_QUIT;
        else if(point_in_rect(mouse_pos, back_bb)) ma = MENU_CLICK_BACK;
        else ma = MENU_NO_ACTION;
    }
    return ma;
}

void menu_init(void) {
    menu_font = LoadFontEx(MENU_FONT, 96, NULL, 0);
    SetTextureFilter(menu_font.texture, TEXTURE_FILTER_BILINEAR);
    // Populate BB rectangles for clickable areas
    // START
    // CONTROLS
    // QUIT
    // BACK
    float top_y = MENU_TOP_Y;
    float padding = 5.0f;
    Vector2 fontWH = MeasureTextEx(menu_font, menu_start, MENU_FONT_SIZE, MENU_SPACING);
    start_bb.x = G_W/2 - fontWH.x/2;
    start_bb.y = top_y;
    start_bb.width = fontWH.x;
    start_bb.height = fontWH.y;

    // Don't increment y yet, RESUME will be at the same place as START
    fontWH = MeasureTextEx(menu_font, menu_pause, MENU_FONT_SIZE, MENU_SPACING);
    pause_bb.x = G_W/2 - fontWH.x/2;
    pause_bb.y = top_y;
    pause_bb.width = fontWH.x;
    pause_bb.height = fontWH.y;

    top_y += fontWH.y + padding;

    fontWH = MeasureTextEx(menu_font, menu_controls, MENU_FONT_SIZE, MENU_SPACING);
    controls_bb.x = G_W/2 - fontWH.x/2;
    controls_bb.y = top_y;
    controls_bb.width = fontWH.x;
    controls_bb.height = fontWH.y;
    top_y += fontWH.y + padding;

    fontWH = MeasureTextEx(menu_font, menu_quit, MENU_FONT_SIZE, MENU_SPACING);
    quit_bb.x = G_W/2 - fontWH.x/2;
    quit_bb.y = top_y;
    quit_bb.width = fontWH.x;
    quit_bb.height = fontWH.y;
    top_y += fontWH.y + padding;

    int control_linecount = 0;
    for(int i = 0; controls_text[i] != NULL; ++i) control_linecount++;

    fontWH = MeasureTextEx(menu_font, menu_back, MENU_FONT_SIZE, MENU_SPACING);
    back_bb.x = G_W/2 - fontWH.x/2;
    back_bb.y = CONTROLS_TOP_Y + ((fontWH.y + padding) * control_linecount);
    back_bb.width = fontWH.x;
    back_bb.height = fontWH.y;
}

void menu_draw(menu_action ma, menu_type mt) {
    if(mstate == MENU_MAIN && ma == MENU_CLICK_CONTROLS) mstate = MENU_CONTROLS;
    if(mstate == MENU_CONTROLS && ma == MENU_CLICK_BACK) mstate = MENU_MAIN;
    if(mstate == MENU_MAIN) {
        if(mt == MENU_START) {
            DrawTextEx(menu_font, menu_start, (Vector2){start_bb.x, start_bb.y}, MENU_FONT_SIZE, MENU_SPACING, WHITE);
        } else {
            DrawTextEx(menu_font, menu_pause, (Vector2){pause_bb.x, pause_bb.y}, MENU_FONT_SIZE, MENU_SPACING, WHITE);
        }
        DrawTextEx(menu_font, menu_controls, (Vector2){controls_bb.x, controls_bb.y}, MENU_FONT_SIZE, MENU_SPACING, WHITE);
        DrawTextEx(menu_font, menu_quit, (Vector2){quit_bb.x, quit_bb.y}, MENU_FONT_SIZE, MENU_SPACING, WHITE);
    } else {
        Vector2 fontWH;
        float top_y = CONTROLS_TOP_Y;
        float padding = 5.0f;
        for(int i = 0; controls_text[i] != NULL; ++i) {
            fontWH = MeasureTextEx(menu_font, controls_text[i], MENU_FONT_SIZE, MENU_SPACING);
            DrawTextEx(menu_font, controls_text[i], (Vector2){G_W/2 - fontWH.x/2, top_y}, MENU_FONT_SIZE, MENU_SPACING, WHITE);
            top_y += fontWH.y + padding;
        }
        DrawTextEx(menu_font, menu_back, (Vector2){back_bb.x, back_bb.y}, MENU_FONT_SIZE, MENU_SPACING, WHITE);
    }

}