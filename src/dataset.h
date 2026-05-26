#ifndef DATASET_H
#define DATASET_H

#include "../lib/dynarray/dynarray.h"
#include "../lib/raylib/raylib.h"
#include "../lib/raylib/raymath.h"
#include <stdlib.h>

typedef struct DataSet {
    dynarray_t * data;
} data_set_t;

// #ifdef DATASET_IMPLEMENTATION

static data_set_t * dataset_init_empty() {
    data_set_t * set = (data_set_t *)malloc(sizeof(data_set_t));
    set->data = arr(Vector2);
    return set;
}

static data_set_t * dataset_create() {
    data_set_t * set = dataset_init_empty();
    srand(1747);
    int n = 1000;
    // push consensus data
    for (int i = 0; i < n; i++) {
        Vector2 vec = {
            (float)(rand()%100 + 20),
            (float)(rand()%100 + 20)
        };
        arr_push(Vector2,set->data,&vec);
    }
    // push noise
    // push first cluster
    for (int i = 0; i < n/10; i++) {
        Vector2 vec = {
            (float)(rand()%20),
            (float)(rand()%20 + 60)
        };
        arr_push(Vector2,set->data,&vec);
    }
    // push second cluster
    for (int i = 0; i < n/10; i++) {
        Vector2 vec = {
            (float)(rand()%20 + 120),
            (float)(rand()%20)
        };
        arr_push(Vector2,set->data,&vec);
    }

    return set;
}

static void dataset_destroy(data_set_t * set) {
    arr_free(set->data);
    free(set);
}

// #endif

#endif