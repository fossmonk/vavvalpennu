#ifndef _MOVIE_H_
#define _MOVIE_H_

#include <raylib.h>

// Movies are basically animations which don't interact with users,
// They can be triggered by play_movie which will make that movie 
// active in the list of movie frames cycled through
// This is similar to all sprite animations
// Just a special case

typedef struct {
    const char* spritesheet_fname;
    int num_frames;
    float frame_time;
} movie_info_t;

// this is where the spritesheet loads to GPU
// returning internal array index
int movie_register(movie_info_t m_info);
// This will make the movie active in the list
void play_movie(int movie_id, Vector2 pos, bool repeat);
// This will stop anim advance for that movie
// But the frame will be drawn
void pause_movie(int movie_id);
// Set movie inactive;
void stop_movie(int movie_id);
Vector2 movie_get_framedim(int movie_id);

void movie_update_all(float dt);
void movie_draw_all(void);

#endif