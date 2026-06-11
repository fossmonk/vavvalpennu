#include <raylib.h>

#ifndef _MENU_H_
#define _MENU_H_

typedef enum {
    MENU_NO_ACTION,
    MENU_CLICK_START,
    MENU_CLICK_RESUME,
    MENU_CLICK_CONTROLS,
    MENU_CLICK_QUIT,
    MENU_CLICK_BACK,
} menu_action;

typedef enum {
    MENU_START,
    MENU_PAUSE
} menu_type;

void menu_init(void);
menu_action menu_get_action(void);
void menu_draw(menu_action ma, menu_type mt);

#endif