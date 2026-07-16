#include <stdlib.h>
#include <math.h>
#include <raylib.h>
#include <movie.h>
#include <anim.h>

#define MAX_MOVIES (20)

typedef struct {
    Texture2D tex;
    int framecount;
    float frametime;
    float timer;
    Rectangle currframe;
    Vector2 pos;
    bool active;
    bool loop;
    bool paused;
} movie_t;

static movie_t* movies[MAX_MOVIES] = { NULL };
static int movie_idx = 0;

int movie_register(movie_info_t m_info) {
    int ret = -1;

    movie_t *m = (movie_t *)malloc(sizeof(movie_t));
    m->tex = LoadTexture(m_info.spritesheet_fname);
    m->framecount = m_info.num_frames;
    m->frametime = m_info.frame_time;
    m->active = false;
    m->loop = false;
    m->paused = false;
    float w = (m->tex.width / m->framecount);
    float h = m->tex.height;
    m->currframe = (Rectangle){0, 0, w, h};

    if(movie_idx < MAX_MOVIES) {
        movies[movie_idx] = m;
        ret = movie_idx;
        movie_idx++;
    }

    return ret;
}

Vector2 movie_get_framedim(int movie_id) {
    Vector2 dim;
    dim.x = movies[movie_id]->currframe.width;
    dim.y = movies[movie_id]->currframe.height;
    return dim;
}

void play_movie(int movie_id, Vector2 pos, bool loop) {
    movie_t *m = movies[movie_id];
    if(m) {
        m->pos = pos;
        m->active = true;
        m->paused = false;
        m->loop = loop;
    }
}

void pause_movie(int movie_id) {
    movie_t *m = movies[movie_id];
    if(m) m->paused = true;
}

void stop_movie(int movie_id) {
    movie_t *m = movies[movie_id];
    if(m) {
        m->active = false;
        m->paused = false;
    }
}

void movie_update(movie_t *m, float dt) {
    if(!m->paused) {
        // update frame 
        m->timer += dt;
        if(m->timer >= m->frametime) {
            int tw = m->tex.width;
            int fw = m->currframe.width;
            int curr_x = m->currframe.x;

            curr_x = m->loop ? (curr_x + fw) % tw : (int)fminf(curr_x + fw, tw-fw);

            m->currframe.x = curr_x;
            m->timer -= m->frametime;

            if(curr_x == (tw-fw) && !m->loop) {
                // end the movie automatically
                m->active = false;
                m->currframe.x = 0;
            }
        }
    }
}

void movie_update_all(float dt) {
    for(int i = 0; i < MAX_MOVIES; ++i) {
        movie_t *m = movies[i];
        if(m && m->active) {
            movie_update(m, dt);
        }
    }
}

void movie_draw_all(void) {
    for(int i = 0; i < MAX_MOVIES; ++i) {
        movie_t *m = movies[i];
        if(m && m->active) {
            DrawTextureRec(m->tex, m->currframe, m->pos, WHITE);
        }
    }
}

void movie_unregister(int id) {
    free(movies[movie_idx]);
    movies[movie_idx] = NULL;
}
