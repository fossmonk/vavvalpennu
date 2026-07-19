#ifndef _DYN_ARR_H_
#define _DYN_ARR_H_

#include <stdlib.h>

typedef struct {
    size_t count;
    size_t capacity;
} dyn_arr_header_t;

#define DA_INIT_CAP (4)

#define dyn_arr_append(arr, x) do {                                                               \
    if (arr == NULL) {                                                                            \
        dyn_arr_header_t *h = malloc(DA_INIT_CAP * sizeof(*arr) + sizeof(dyn_arr_header_t)); \
        h->capacity = DA_INIT_CAP;                                                           \
        h->count = 0;                                                                             \
        arr = (void *)(h + 1);                                                                    \
    }                                                                                             \
    dyn_arr_header_t *h = (dyn_arr_header_t *)arr - 1;                                            \
    if(h->count >= h->capacity) {                                                                 \
        h->capacity *= 2;                                                                         \
        h = realloc(h, (h->capacity)*(sizeof(*arr)) + sizeof(dyn_arr_header_t));                  \
        arr = (void *)(h + 1);                                                                    \
    }                                                                                             \
    arr[h->count++] = (x);                                                                        \
} while (0)

#define dyn_arr_free(arr) do {                                                                    \
    if(arr != NULL) {                                                                             \
        free((dyn_arr_header_t *)arr - 1);                                                        \
    }                                                                                             \
} while(0)

#define dyn_arr_len(arr) (arr != NULL ? (((dyn_arr_header_t *)arr - 1)->count) : -1)

#endif