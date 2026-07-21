#include <stdlib.h>
#include <raylib.h>
#include <vpconfig.h>
#include <menu.h>
#include <input.h>

#define TOP_Y (0.25 * G_H)
#define MENU_FONT_SIZE (35)
#define MENU_SPACING (0.75)
#define ITEM_PADDING (12.0f)

typedef enum {
    MENU_MAIN,
    MENU_CONTROLS
} menu_state;

static menu_state mstate = MENU_MAIN;
static Font menu_font;
static int g_reb_idx = -1;
static bool g_skip_input_frame = false; // Ignore inputs on the frame rebind opens

static Vector2 get_scaled_mouse_pos(void) {
    Vector2 mouseScalePos = GetMousePosition();
    mouseScalePos.x = (mouseScalePos.x / GetScreenWidth()) * G_W;
    mouseScalePos.y = (mouseScalePos.y / GetScreenHeight()) * G_H;
    return mouseScalePos;
}

static bool point_in_rect(Vector2 p, Rectangle r) {
    return (p.x > r.x && p.x < r.x + r.width && p.y > r.y && p.y < r.y + r.height);
}

static Vector2 menu_measure_text(const char *text) {
    return MeasureTextEx(menu_font, text, MENU_FONT_SIZE, MENU_SPACING);
}

static void menu_draw_text(const char* text, Vector2 position, Color color) {
    DrawTextEx(menu_font, text, position, MENU_FONT_SIZE, MENU_SPACING, color);
}

static bool menu_draw_button_and_check_clicked(const char *text, float cx, float y) {
    Vector2 font_wh = menu_measure_text(text);
    Rectangle bb = {cx - font_wh.x/2.0f, y, font_wh.x, font_wh.y};

    Vector2 mouse_pos = get_scaled_mouse_pos();
    bool hovered = point_in_rect(mouse_pos, bb);
    Color color = hovered ? YELLOW : WHITE;

    menu_draw_text(text, (Vector2){bb.x, bb.y}, color);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static bool menu_draw_ctrl_row_and_check_clicked(const char *action, const char *key, float xl, float xr, float y, bool is_rebinding) {
    const char* disptext = is_rebinding ? "PRESS ANY KEY..." : key;
    Vector2 disp_wh = menu_measure_text(disptext);
    Rectangle row_bb = {xl, y, xr - xl, disp_wh.y};
    Vector2 mousepos = get_scaled_mouse_pos();
    bool hovered = point_in_rect(mousepos, row_bb);
    
    Color action_color = hovered ? YELLOW : WHITE;
    Color disp_color = is_rebinding ? RED : (hovered ? YELLOW : LIGHTGRAY);
    
    menu_draw_text(action, (Vector2){xl, y}, action_color);
    menu_draw_text(disptext, (Vector2){xr - disp_wh.x, y}, disp_color);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

static bool key_taken(input_device d, int key, input_cfg_t *icfg, int idx) {
    input_item_t *controls[] = {
        &icfg->move_left, &icfg->move_right, &icfg->jump,
        &icfg->lash_whip, &icfg->throw_batr,
    };
    int controls_size = ARRAY_SIZE(controls);

    for (int i = 0; i < controls_size; ++i) {
        if (controls[i]->device == d && controls[i]->value == key && i != idx) {
            return true;
        }
    }
    return false;
}

static void menu_handle_keybinding(input_cfg_t *icfg) {
    if (g_reb_idx < 0) return;

    // Discard input on the frame rebind mode was clicked
    if (g_skip_input_frame) {
        g_skip_input_frame = false;
        return;
    }

    int kb = GetKeyPressed();
    int mb = -1;
    for (int i = MOUSE_BUTTON_LEFT; i <= MOUSE_BUTTON_BACK; ++i) {
        if (IsMouseButtonPressed(i)) {
            mb = i;
            break;
        }
    }

    if (kb <= 0 && mb < 0) return;

    input_item_t *controls[] = {
        &icfg->move_left, &icfg->move_right, &icfg->jump,
        &icfg->lash_whip, &icfg->throw_batr,
    };
    int controls_size = ARRAY_SIZE(controls);

    // Prioritize mouse button rebind
    if (mb >= 0 && g_reb_idx < controls_size) {
        if (!key_taken(MOUSE, mb, icfg, g_reb_idx)) {
            controls[g_reb_idx]->device = MOUSE;
            controls[g_reb_idx]->value = mb;
            controls[g_reb_idx]->name = input_get_mouse_button_name(mb);
            input_cfg_sync_to_file();
        }
        g_reb_idx = -1;
    } 
    // Keyboard key rebind
    else if (kb > 0 && g_reb_idx < controls_size) {
        if (kb == KEY_ESCAPE) {
            g_reb_idx = -1;
            return;
        }

        if (!key_taken(KEYBOARD, kb, icfg, g_reb_idx)) {
            controls[g_reb_idx]->device = KEYBOARD;
            controls[g_reb_idx]->value = kb;
            controls[g_reb_idx]->name = input_get_kb_keyname(kb);
            input_cfg_sync_to_file();
        }
        g_reb_idx = -1;
    }
}

void menu_init(void) {
    menu_font = LoadFontEx(MENU_FONT, 96, NULL, 0);
    SetTextureFilter(menu_font.texture, TEXTURE_FILTER_BILINEAR);
}

menu_action menu_update_and_draw(menu_type mt) {
    menu_action action_trig = MENU_NO_ACTION;
    input_cfg_t *icfg = input_get_cfg();

    float start_y = TOP_Y;
    float cx = G_W / 2.0f;

    if (mstate == MENU_MAIN) {
        const char *first_option = (mt == MENU_START) ? "START GAME" : "RESUME GAME";

        if (menu_draw_button_and_check_clicked(first_option, cx, start_y)) {
            action_trig = (mt == MENU_START) ? MENU_CLICK_START : MENU_CLICK_RESUME;
        }
        start_y += MENU_FONT_SIZE + ITEM_PADDING;

        if (menu_draw_button_and_check_clicked("CONTROLS", cx, start_y)) {
            mstate = MENU_CONTROLS;
        }
        start_y += MENU_FONT_SIZE + ITEM_PADDING;

        if (menu_draw_button_and_check_clicked("QUIT GAME", cx, start_y)) {
            action_trig = MENU_CLICK_QUIT;
        }
    } else if (mstate == MENU_CONTROLS) {
        Vector2 heading_wh = menu_measure_text("CONTROLS");
        menu_draw_text("CONTROLS", (Vector2){cx - heading_wh.x/2.0f, start_y}, WHITE);
        start_y += MENU_FONT_SIZE + ITEM_PADDING * 2;

        input_item_t *controls[] = {
            &icfg->move_left,
            &icfg->move_right,
            &icfg->jump,
            &icfg->lash_whip,
            &icfg->throw_batr
        };
        int controls_size = ARRAY_SIZE(controls);

        float margin_x = 0.3 * G_W;
        float xl = margin_x;
        float xr = G_W - margin_x;

        for (int i = 0; i < controls_size; ++i) {
            bool is_rebinding = (g_reb_idx == i);
            bool ctrl_row_clicked = menu_draw_ctrl_row_and_check_clicked(
                controls[i]->description, 
                controls[i]->name, 
                xl, xr, start_y, 
                is_rebinding
            );

            // Only check row clicks if NOT currently rebinding
            if (g_reb_idx == -1 && ctrl_row_clicked) {
                g_reb_idx = i;
                g_skip_input_frame = true; // Ignore initial click in key handler
            }
            start_y += MENU_FONT_SIZE + ITEM_PADDING;
        }

        start_y += ITEM_PADDING * 2;
        if (g_reb_idx == -1 && menu_draw_button_and_check_clicked("GO BACK", cx, start_y)) {
            mstate = MENU_MAIN;
        }
    }

    // Capture keybinding AFTER UI rendering pass
    if (g_reb_idx >= 0) {
        menu_handle_keybinding(icfg);
    }

    return action_trig;
}