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

typedef struct {
    const char *action;
    const char *control;
} cp;

static menu_state mstate = MENU_MAIN;
static const char *menu_start = "START GAME";
static const char *menu_pause = "RESUME GAME";
static const char *menu_controls = "CONTROLS";
static const char *menu_quit = "QUIT GAME";
static const char *menu_back = "GO BACK";

static const char* controls_heading[] = {
    "CONTROLS",
    "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
    NULL
};

static cp control_pairs[] = {
    {"MOVE LEFT","A"},
    {"MOVE RIGHT","D"},
    {"JUMP","SPACE"},
    {"THROW BATARANG","RMB"},
    {"HEAVY ATTACK","LMB"},
    {NULL, NULL}
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

    return (p.x > xl && p.x < xh && p.y > yl && p.y < yh);
}

// Since we can't curry let's save on some typing
Vector2 menu_measure_text(const char *text) {
    return MeasureTextEx(menu_font, text, MENU_FONT_SIZE, MENU_SPACING);
}

void menu_draw_text(const char* text, Vector2 position) {
    DrawTextEx(menu_font, text, position, MENU_FONT_SIZE, MENU_SPACING, WHITE);
}

menu_action menu_get_action(void) {
    menu_action ma = MENU_NO_ACTION;
    bool mousep = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
    #ifdef __EMSCRIPTEN__
    mousep = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    #endif
    if(mousep) {
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
    Vector2 fontWH = menu_measure_text(menu_start);
    start_bb.x = G_W/2 - fontWH.x/2;
    start_bb.y = top_y;
    start_bb.width = fontWH.x;
    start_bb.height = fontWH.y;

    // Don't increment y yet, RESUME will be at the same place as START
    fontWH = menu_measure_text(menu_pause);
    pause_bb.x = G_W/2 - fontWH.x/2;
    pause_bb.y = top_y;
    pause_bb.width = fontWH.x;
    pause_bb.height = fontWH.y;

    top_y += fontWH.y + padding;

    fontWH = menu_measure_text(menu_controls);
    controls_bb.x = G_W/2 - fontWH.x/2;
    controls_bb.y = top_y;
    controls_bb.width = fontWH.x;
    controls_bb.height = fontWH.y;
    top_y += fontWH.y + padding;

    fontWH = menu_measure_text(menu_quit);
    quit_bb.x = G_W/2 - fontWH.x/2;
    quit_bb.y = top_y;
    quit_bb.width = fontWH.x;
    quit_bb.height = fontWH.y;
    top_y += fontWH.y + padding;

    int control_linecount = 0;
    for(int i = 0; controls_heading[i] != NULL; ++i) control_linecount++;
    for(int i = 0; control_pairs[i].action != NULL; ++i) control_linecount++;

    fontWH = menu_measure_text(menu_back);
    back_bb.x = G_W/2 - fontWH.x/2;
    back_bb.y = CONTROLS_TOP_Y + ((fontWH.y + padding) * control_linecount);
    back_bb.width = fontWH.x;
    back_bb.height = fontWH.y;
}

void menu_draw_controls(void) {
    Vector2 fontWH;
    float top_y = CONTROLS_TOP_Y;
    float padding = 5.0f;
    // Draw CONTROLS
    //      ========
    for(int i = 0; i < 2; ++i) {
        fontWH = menu_measure_text(controls_heading[i]);
        menu_draw_text(controls_heading[i], (Vector2){G_W/2 - fontWH.x/2, top_y});
        top_y += fontWH.y + padding;
    }
    float xl = G_W/2 - fontWH.x/2;
    float xr = xl + fontWH.x;

    for(int i = 0; control_pairs[i].action != NULL; ++i) {
        menu_draw_text(control_pairs[i].action, (Vector2){xl, top_y});
        fontWH = menu_measure_text(control_pairs[i].control);
        menu_draw_text(control_pairs[i].control, (Vector2){xr - fontWH.x, top_y});
        top_y += fontWH.y + padding;
    }
}

void menu_draw(menu_action ma, menu_type mt) {
    if(mstate == MENU_MAIN && ma == MENU_CLICK_CONTROLS) mstate = MENU_CONTROLS;
    if(mstate == MENU_CONTROLS && ma == MENU_CLICK_BACK) mstate = MENU_MAIN;
    if(mstate == MENU_MAIN) {
        if(mt == MENU_START) {
            menu_draw_text(menu_start, (Vector2){start_bb.x, start_bb.y});
        } else {
            menu_draw_text(menu_pause, (Vector2){pause_bb.x, pause_bb.y});
        }
        menu_draw_text(menu_controls, (Vector2){controls_bb.x, controls_bb.y});
        menu_draw_text(menu_quit, (Vector2){quit_bb.x, quit_bb.y});
    } else {
        menu_draw_controls();
        menu_draw_text(menu_back, (Vector2){back_bb.x, back_bb.y});
    }
}