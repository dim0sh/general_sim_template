#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stddef.h>

typedef struct DynArray {
    size_t capacity;
    size_t cnt;
    void * data;
} dynarray_t;

#define MIN_CAPACITY 100

#define arr(T) (dynarray_t *)_array_init(sizeof(T), MIN_CAPACITY)
dynarray_t *_array_init(size_t size, size_t init_capacity);

#define arr_push(T,a,b) _array_push(sizeof(T),a,b, MIN_CAPACITY)
void _array_push(size_t size, dynarray_t *arr, void *elem, size_t init_capacity);

#define arr_pop(T,a) (T *)_array_pop(sizeof(T), a, MIN_CAPACITY)
void *_array_pop(size_t size, dynarray_t *arr, size_t init_capacity);

#define arr_set(T,a,b,c) _array_set(sizeof(T),a,b,c)
void _array_set(size_t size, dynarray_t *arr, size_t idx, void *elem);

#define arr_get(T,a,b) (T *)_array_get(sizeof(T),a,b)
void *_array_get(size_t size, dynarray_t *arr, size_t idx);

#define arr_peek(T,a) arr_get(T,a,a->cnt-1)

#define arr_swap_remove(T,a,b) _array_swap_remove(sizeof(T),a,b, MIN_CAPACITY)
void _array_swap_remove(size_t size, dynarray_t *arr, size_t idx, size_t init_capacity);

#define arr_insert(T,a,b,c) _array_insert(sizeof(T),a,b,c, MIN_CAPACITY)
void _array_insert(size_t size, dynarray_t *arr, size_t idx, void *elem, size_t init_capacity);

#define arr_remove(T,a,b) _array_remove(sizeof(T),a,b, MIN_CAPACITY)
void _array_remove(size_t size, dynarray_t *arr, size_t idx, size_t init_capacity);

// #define arr_pop_front(T,a) (T *)_array_pop_front(sizeof(T),a, MIN_CAPACITY)
// void *_array_pop_front(size_t size, dynarray_t *arr, size_t init_capacity);

#define arr_push_front(T,a,b) arr_insert(T,a,0,b)

#define arr_concat(T,a,b) _array_concat(sizeof(T),a,b)
void _array_concat(size_t size, dynarray_t *dest, dynarray_t *other);

#define arr_free(a) _array_free(a)
void _array_free(dynarray_t *arr);

#endif