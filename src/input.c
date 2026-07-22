#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <input.h>
#include <vpconfig.h>

bool mouse_down[8]  = { false };
bool keys_down[400] = { false };

#define INIT_MOVE_RIGHT (input_item_t){"MOVE RIGHT", "D", KEY_D, KEYBOARD}
#define INIT_MOVE_LEFT  (input_item_t){"MOVE LEFT", "A", KEY_A, KEYBOARD}
#define INIT_JUMP       (input_item_t){"JUMP", "SPACE", KEY_SPACE, KEYBOARD}
#define INIT_LASH_WHIP  (input_item_t){"LASH WHIP", "LMB", MOUSE_BUTTON_LEFT, MOUSE}
#define INIT_THROW_BATR (input_item_t){"THROW BATARANG", "RMB", MOUSE_BUTTON_RIGHT, MOUSE}

static input_cfg_t g_input_cfg;
static bool input_init_done = false;

static char* g_input_name_map_kb[] = {
    // Alphanumeric keys
    [KEY_APOSTROPHE]      = "'", 
    [KEY_COMMA]           = ",", 
    [KEY_MINUS]           = "-", 
    [KEY_PERIOD]          = ".", 
    [KEY_SLASH]           = "/", 
    [KEY_ZERO]            = "0", 
    [KEY_ONE]             = "1", 
    [KEY_TWO]             = "2", 
    [KEY_THREE]           = "3", 
    [KEY_FOUR]            = "4", 
    [KEY_FIVE]            = "5", 
    [KEY_SIX]             = "6", 
    [KEY_SEVEN]           = "7", 
    [KEY_EIGHT]           = "8", 
    [KEY_NINE]            = "9", 
    [KEY_SEMICOLON]       = ";", 
    [KEY_EQUAL]           = "=", 
    [KEY_A]               = "A", 
    [KEY_B]               = "B", 
    [KEY_C]               = "C", 
    [KEY_D]               = "D", 
    [KEY_E]               = "E", 
    [KEY_F]               = "F", 
    [KEY_G]               = "G", 
    [KEY_H]               = "H", 
    [KEY_I]               = "I", 
    [KEY_J]               = "J", 
    [KEY_K]               = "K", 
    [KEY_L]               = "L", 
    [KEY_M]               = "M", 
    [KEY_N]               = "N", 
    [KEY_O]               = "O", 
    [KEY_P]               = "P", 
    [KEY_Q]               = "Q", 
    [KEY_R]               = "R", 
    [KEY_S]               = "S", 
    [KEY_T]               = "T", 
    [KEY_U]               = "U", 
    [KEY_V]               = "V", 
    [KEY_W]               = "W", 
    [KEY_X]               = "X", 
    [KEY_Y]               = "Y", 
    [KEY_Z]               = "Z", 
    [KEY_LEFT_BRACKET]    = "[", 
    [KEY_BACKSLASH]       = "\\",
    [KEY_RIGHT_BRACKET]   = "]", 
    [KEY_GRAVE]           = "`", 
    // Function keys
    [KEY_SPACE]           = "SPACE",
    [KEY_ESCAPE]          = "ESC",
    [KEY_ENTER]           = "ENTER",
    [KEY_TAB]             = "TAB",
    [KEY_BACKSPACE]       = "BACKSPACE",
    [KEY_INSERT]          = "INSERT",
    [KEY_DELETE]          = "DELETE",
    [KEY_RIGHT]           = "RIGHT ARROW",
    [KEY_LEFT]            = "LEFT ARROW",
    [KEY_DOWN]            = "DOWN ARROW",
    [KEY_UP]              = "UP ARROW",
    [KEY_PAGE_UP]         = "PGUP",
    [KEY_PAGE_DOWN]       = "PGDN",
    [KEY_HOME]            = "HOME",
    [KEY_END]             = "END",
    [KEY_CAPS_LOCK]       = "CAPSLOCK",
    [KEY_SCROLL_LOCK]     = "SCROLL LOCK",
    [KEY_NUM_LOCK]        = "NUMLOCK",
    [KEY_PRINT_SCREEN]    = "PRNTSCRN",
    [KEY_F1]              = "F1",
    [KEY_F2]              = "F2",
    [KEY_F3]              = "F3",
    [KEY_F4]              = "F4",
    [KEY_F5]              = "F5",
    [KEY_F6]              = "F6",
    [KEY_F7]              = "F7",
    [KEY_F8]              = "F8",
    [KEY_F9]              = "F9",
    [KEY_F10]             = "F10",
    [KEY_F11]             = "F11",
    [KEY_F12]             = "F12",
    [KEY_LEFT_SHIFT]      = "LSHIFT",
    [KEY_LEFT_CONTROL]    = "LCTRL",
    [KEY_LEFT_ALT]        = "LALT",
    [KEY_LEFT_SUPER]      = "LWIN",
    [KEY_RIGHT_SHIFT]     = "RSHIFT",
    [KEY_RIGHT_CONTROL]   = "RCTRL",
    [KEY_RIGHT_ALT]       = "RALT",
    [KEY_RIGHT_SUPER]     = "RWIN",
    [KEY_KB_MENU]         = "KBMENU",
    // Keypad keys
    [KEY_KP_0]            = "NUMPAD0",
    [KEY_KP_1]            = "NUMPAD1",
    [KEY_KP_2]            = "NUMPAD2",
    [KEY_KP_3]            = "NUMPAD3",
    [KEY_KP_4]            = "NUMPAD4",
    [KEY_KP_5]            = "NUMPAD5",
    [KEY_KP_6]            = "NUMPAD6",
    [KEY_KP_7]            = "NUMPAD7",
    [KEY_KP_8]            = "NUMPAD8",
    [KEY_KP_9]            = "NUMPAD9",
    [KEY_KP_DECIMAL]      = "NUMPAD .",
    [KEY_KP_DIVIDE]       = "NUMPAD /",
    [KEY_KP_MULTIPLY]     = "NUMPAD *",
    [KEY_KP_SUBTRACT]     = "NUMPAD -",
    [KEY_KP_ADD]          = "NUMPAD +",
    [KEY_KP_ENTER]        = "NUMPAD ENTER",
    [KEY_KP_EQUAL]        = "NUMPAD =",
};

static char* g_input_name_map_mouse[] = {
    [MOUSE_BUTTON_LEFT]      = "LMB",
    [MOUSE_BUTTON_RIGHT]     = "RMB",
    [MOUSE_BUTTON_MIDDLE]    = "MMB",
    [MOUSE_BUTTON_SIDE]      = "SIDEMB",
    [MOUSE_BUTTON_EXTRA]     = "EXTRAMB",
    [MOUSE_BUTTON_FORWARD]   = "FWDMB",
    [MOUSE_BUTTON_BACK]      = "BACKMB"
};

bool input_iskeypressed(KeyboardKey k) {
    bool keydown = IsKeyDown(k);
    bool ret = keydown && !keys_down[k];
    keys_down[k] = keydown;
    return ret;
}

bool input_ismousepressed(MouseButton m) {
    bool mousedown = IsMouseButtonDown(m);
    bool ret = mousedown && !mouse_down[m];
    mouse_down[m] = mousedown;
    return ret;
}

void input_cfg_sync_to_file(void) {
    SaveFileData(INPUT_CFG_FILE, (void *)&g_input_cfg, sizeof(g_input_cfg));
}

void input_cfg_load_strs(void) {
    g_input_cfg.move_left.description   = INIT_MOVE_LEFT.description;
    g_input_cfg.move_right.description  = INIT_MOVE_RIGHT.description;
    g_input_cfg.jump.description        = INIT_JUMP.description;
    g_input_cfg.lash_whip.description   = INIT_LASH_WHIP.description;
    g_input_cfg.throw_batr.description  = INIT_THROW_BATR.description;

    input_item_t *items[] = {
        &g_input_cfg.move_left,
        &g_input_cfg.move_right,
        &g_input_cfg.jump,
        &g_input_cfg.lash_whip,
        &g_input_cfg.throw_batr,
    };

    for(int i = 0; i < ARRAY_SIZE(items); ++i) {
        switch (items[i]->device)
        {
            case KEYBOARD:
                items[i]->name = g_input_name_map_kb[items[i]->value];
                break;
            case MOUSE:
                items[i]->name = g_input_name_map_mouse[items[i]->value];
                break;
            
            default:
                items[i]->name = "UNKNOWN";
                break;
        }
    }
}

void input_init(void) {
    g_input_cfg.move_left   = INIT_MOVE_LEFT;
    g_input_cfg.move_right  = INIT_MOVE_RIGHT;
    g_input_cfg.jump        = INIT_JUMP;
    g_input_cfg.lash_whip   = INIT_LASH_WHIP;
    g_input_cfg.throw_batr  = INIT_THROW_BATR;
    if(!FileExists(INPUT_CFG_FILE)) {
        // create file with default config
        input_cfg_sync_to_file();
    } else {
        // load from file
        int sz;
        unsigned char* data = LoadFileData(INPUT_CFG_FILE, &sz);
        if(sz == sizeof(g_input_cfg)) {
            unsigned char *p = (unsigned char *)&g_input_cfg;
            unsigned char *q = data;
            for(int i = 0; i < sz; i++) {
                *p++ = *q++;
            }
            // now the strings are not loaded. Populate that
            input_cfg_load_strs();
            UnloadFileData(data);
        } else {
            // cfg file is tampered, dump default config
            input_cfg_sync_to_file();
        }
    }
    input_init_done = true;
}

input_cfg_t * input_get_cfg(void) {
    if(!input_init_done) return NULL;
    return &g_input_cfg;
}

Vector2 input_get_mouse_pos(void) {
    Vector2 mouseScalePos = GetMousePosition();
    mouseScalePos.x = (mouseScalePos.x / GetScreenWidth()) * G_W;
    mouseScalePos.y = (mouseScalePos.y / GetScreenHeight()) * G_H;
    return mouseScalePos;
}

const char * input_get_kb_keyname(KeyboardKey k) {
    return g_input_name_map_kb[k];
}

const char * input_get_mouse_button_name(MouseButton m) {
    return g_input_name_map_mouse[m];
}

bool input_move_right(void) {
    input_item_t *i = &g_input_cfg.move_right;
    bool ret = i->device == KEYBOARD ? IsKeyDown(i->value) : input_ismousepressed(i->value); 
    return ret;
}

bool input_move_left(void) {
    input_item_t *i = &g_input_cfg.move_left;
    bool ret = i->device == KEYBOARD ? IsKeyDown(i->value) : input_ismousepressed(i->value); 
    return ret;
}

bool input_jump(void) {
    input_item_t *i = &g_input_cfg.jump;
    bool ret = i->device == KEYBOARD ? input_iskeypressed(i->value) : input_ismousepressed(i->value); 
    return ret;
}

bool input_lash_whip(void) {
    input_item_t *i = &g_input_cfg.lash_whip;
    bool ret = i->device == KEYBOARD ? input_iskeypressed(i->value) : input_ismousepressed(i->value); 
    return ret;
}

bool input_throw_batr(void) {
    input_item_t *i = &g_input_cfg.throw_batr;
    bool ret = i->device == KEYBOARD ? input_iskeypressed(i->value) : input_ismousepressed(i->value); 
    return ret;
}

