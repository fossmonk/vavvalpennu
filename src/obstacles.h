#ifndef _OBSTACLES_H
#define _OBSTACLES_H

#include <stdlib.h>

typedef struct {
    size_t count;
    size_t capacity;
} obs_da_header_t;

#define OBS_DA_INIT_CAP (4)

#define obs_da_append(arr, x) do {                                                               \
    if (arr == NULL) {                                                                            \
        obs_da_header_t *h = malloc(OBS_DA_INIT_CAP * sizeof(*arr) + sizeof(obs_da_header_t)); \
        h->capacity = OBS_DA_INIT_CAP;                                                           \
        h->count = 0;                                                                             \
        arr = (void *)(h + 1);                                                                    \
    }                                                                                             \
    obs_da_header_t *h = (obs_da_header_t *)arr - 1;                                            \
    if(h->count >= h->capacity) {                                                                 \
        h->capacity *= 2;                                                                         \
        h = realloc(h, (h->capacity)*(sizeof(*arr)) + sizeof(obs_da_header_t));                  \
        arr = (void *)(h + 1);                                                                    \
    }                                                                                             \
    arr[h->count++] = (x);                                                                        \
} while (0)

#define obs_da_free(arr) do {                                                                    \
    if(arr != NULL) {                                                                             \
        free((obs_da_header_t *)arr - 1);                                                        \
    }                                                                                             \
} while(0)

#define obs_da_len(arr) (arr != NULL ? (((obs_da_header_t *)arr - 1)->count) : -1)

#endif