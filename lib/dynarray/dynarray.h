#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <string.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct DynArray {
    size_t capacity;
    size_t cnt;
    char * data;
} dynarray_t;

#define MIN_CAPACITY 100
__attribute__((warn_unused_result,malloc))
static inline dynarray_t *_array_empty() {
    dynarray_t *result = (dynarray_t *)malloc(sizeof(dynarray_t));

    memset(result, 0, sizeof(dynarray_t));
    return result;
}
#define arr(T) (dynarray_t *)_array_init(sizeof(T), MIN_CAPACITY)
__attribute__((warn_unused_result,malloc))
static inline dynarray_t *_array_init(size_t size, size_t init_capacity) {
    dynarray_t *ptr = _array_empty();
    ptr->data = (char *)malloc(size*init_capacity);
    // memset(ptr->data, 0, size*init_capacity);
    ptr->capacity = init_capacity;
    return ptr;
}
__attribute__((nonnull(2)))
static inline void _array_resize(size_t size, dynarray_t *arr, int direction, size_t init_capacity) {
    switch (direction) {
        case 1:
            arr->data = (char *)realloc(arr->data, size*arr->capacity*2);
            arr->capacity = arr->capacity*2;
            break;
        case 0:
            if (arr->capacity >= init_capacity * 2) {
                arr->data = (char *)realloc(arr->data, size*(arr->capacity/2));
                arr->capacity = arr->capacity/2;
            }
            break;
    }   
}

#define arr_push(T,a,b) _array_push(sizeof(T),a,b, MIN_CAPACITY)
__attribute((hot,nonnull(2)))
static inline void _array_push(size_t size, dynarray_t *arr, void *elem, size_t init_capacity) {
    if (arr->cnt < arr->capacity) {
        char *ptr = arr->data;
        ptr = ptr + (size * arr->cnt);
        memcpy(ptr, elem, size);
        arr->cnt++;
    } else {
        _array_resize(size, arr, 1, init_capacity);
        _array_push(size, arr, elem, init_capacity);
    }
}

#define arr_pop(T,a) (T *)_array_pop(sizeof(T), a, MIN_CAPACITY)
__attribute__((warn_unused_result,nonnull(2)))
static inline void *_array_pop(size_t size, dynarray_t *arr, size_t init_capacity) {
    char *ptr = NULL;
    if (arr->cnt > 0) {
        ptr = arr->data + size * (arr->cnt - 1);
        arr->cnt--;
        if (arr->cnt < arr->capacity/3) {
            _array_resize(size, arr, 0, init_capacity);
        }
    }
    return ptr;
}

#define arr_set(T,a,b,c) _array_set(sizeof(T),a,b,c)
__attribute((nonnull(2)))
static inline void _array_set(size_t size, dynarray_t *arr, size_t idx, void *elem) {
    if (idx < arr->cnt) {
        char *ptr = arr->data;
        ptr = ptr + (idx * size);
        memcpy(ptr, elem, size);
    }
}

#define arr_get(T,a,b) (T *)_array_get(sizeof(T),a,b)
__attribute__((hot,warn_unused_result,hot,nonnull(2)))
static inline void *_array_get(size_t size, dynarray_t *arr, size_t idx) {
    char *ptr = NULL;
    if (idx < arr->cnt) {
        ptr = arr->data + (idx * size);
    }
    return ptr;
}

#define arr_peek(T,a) arr_get(T,a,a->cnt-1)

#define arr_swap_remove(T,a,b) _array_swap_remove(sizeof(T),a,b, MIN_CAPACITY)
__attribute__((nonnull(2)))
static inline void _array_swap_remove(size_t size, dynarray_t *arr, size_t idx, size_t init_capacity) {
    if (arr->cnt > 0) {
        if (idx == arr->cnt-1) {
            arr->cnt--;
        } else if (idx < arr->cnt) {
            char*ptr_dest = arr->data;
            ptr_dest = ptr_dest + (idx * size);
    
            char*ptr_src = arr->data;
            ptr_src = ptr_src + (arr->cnt-1)*size;
    
            memcpy(ptr_dest, ptr_src, size);
            arr->cnt--;
        }
        if (arr->cnt < arr->capacity/3) {
                _array_resize(size, arr, 0, init_capacity);
        }
    }

}

#define arr_insert(T,a,b,c) _array_insert(sizeof(T),a,b,c, MIN_CAPACITY)
__attribute__((nonnull(2,4)))
static inline void _array_insert(size_t size, dynarray_t *arr, size_t idx, void *elem, size_t init_capacity) {
    if (idx == arr->cnt) {
        _array_push(size, arr, elem, init_capacity);
        return;
    }
    if (idx < arr->cnt) {
        if (arr->cnt < arr->capacity) {
            char*ptr = arr->data;
            char*ptr_src = ptr + (idx * size);
            char*ptr_dest = ptr_src + size;
            memcpy(ptr_dest, ptr_src, size * ((arr->cnt)-idx));
            memcpy(ptr_src, elem, size);
            arr->cnt++;
        } else {
            _array_resize(size, arr, 1, init_capacity);
            _array_insert(size, arr, idx, elem, init_capacity);
        }
    }
}

#define arr_remove(T,a,b) _array_remove(sizeof(T),a,b, MIN_CAPACITY)
__attribute((nonnull(2)))
static inline void _array_remove(size_t size, dynarray_t *arr, size_t idx, size_t init_capacity) {
    if (arr->cnt > 0) {
        if (idx == arr->cnt-1) {
            arr->cnt--;
            return;
        }
        if (idx < arr->cnt-1) {
            char*ptr = arr->data;
            char*ptr_dest = ptr + (idx * size);
            char*ptr_src = ptr_dest + size;
            memcpy(ptr_dest, ptr_src, size * ((arr->cnt-1)-idx));
            arr->cnt--;
        }
        if (arr->cnt < arr->capacity/3) {
                _array_resize(size, arr, 0, init_capacity);
        }
    }
}

// #define arr_pop_front(T,a) (T *)_array_pop_front(sizeof(T),a, MIN_CAPACITY)
// void *_array_pop_front(size_t size, dynarray_t *arr, size_t init_capacity);

#define arr_push_front(T,a,b) arr_insert(T,a,0,b)

#define arr_free(a) _array_free(a)
__attribute((nonnull(1)))
static inline void _array_free(dynarray_t *arr) {
    free(arr->data);
    free(arr);
}

#define arr_concat(T,a,b) _array_concat(sizeof(T),a,b)
__attribute((nonnull(2,3)))
static inline void _array_concat(size_t size, dynarray_t *dest, dynarray_t *other) {
    if (dest->capacity > dest->cnt + other->cnt) {

        char *ptr_dest = dest->data + size * dest->cnt;
        char *ptr_src = other->data;
        memcpy(ptr_dest, ptr_src, size * (other->cnt));

        dest->cnt = dest->cnt + other->cnt;
        _array_free(other);
    } else {
        dest->data = (char *)realloc(dest->data, size*(dest->capacity + other->cnt));
        dest->capacity = dest->capacity + other->cnt;
        _array_concat(size, dest, other);
    }
}

#endif