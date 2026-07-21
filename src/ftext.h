#ifndef _FTEXT_H_
#define _FTEXT_H_

#include <raylib.h>

void ftext_init_all(void);
void ftext_spawn(char *text, Vector2 pos);
void ftext_update_all(float dt);
void ftext_draw_all(void);

#endif