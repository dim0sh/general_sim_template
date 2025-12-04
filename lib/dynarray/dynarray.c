#include "dynarray.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

dynarray_t *_array_empty() {
    dynarray_t *result = (dynarray_t *)malloc(sizeof(dynarray_t));

    memset(result, 0, sizeof(dynarray_t));
    return result;
}

dynarray_t *_array_init(size_t size, size_t init_capacity) {
    dynarray_t *ptr = _array_empty();
    ptr->data = malloc(size*init_capacity);
    // memset(ptr->data, 0, size*init_capacity);
    ptr->capacity = init_capacity;
    return ptr;
}

void _array_resize(size_t size, dynarray_t *arr, int direction, size_t init_capacity) {
    switch (direction) {
        case 1:
            arr->data = realloc(arr->data, size*arr->capacity*2);
            arr->capacity = arr->capacity*2;
            break;
        case 0:
            if (arr->capacity >= init_capacity * 2) {
                arr->data = realloc(arr->data, size*(arr->capacity/2));
                arr->capacity = arr->capacity/2;
            }
            break;
    }   
}

void _array_push(size_t size, dynarray_t *arr, void *elem, size_t init_capacity) {
    if (arr->cnt < arr->capacity) {
        void *ptr = arr->data;
        ptr = ptr + (arr->cnt * size);
        memcpy(ptr, elem, size);
        arr->cnt++;
    } else {
        _array_resize(size, arr, 1, init_capacity);
        _array_push(size, arr, elem, init_capacity);
    }
}

void *_array_pop(size_t size, dynarray_t *arr, size_t init_capacity) {
    void *ptr = NULL;
    if (arr->cnt > 0) {
        ptr = arr->data + (arr->cnt - 1) * size;
        arr->cnt--;
        if (arr->cnt < arr->capacity/3) {
            _array_resize(size, arr, 0, init_capacity);
        }
    }
    return ptr;
}

void _array_set(size_t size, dynarray_t *arr, size_t idx, void *elem) {
    if (idx < arr->cnt) {
        void *ptr = arr->data;
        ptr = ptr + (idx * size);
        memcpy(ptr, elem, size);
    }
}

void *_array_get(size_t size, dynarray_t *arr, size_t idx) {
    void *ptr = NULL;
    if (idx < arr->cnt) {
        ptr = arr->data + (idx * size);
    }
    return ptr;
}

void _array_swap_remove(size_t size, dynarray_t *arr, size_t idx, size_t init_capacity) {
    if (arr->cnt > 0) {
        if (idx == arr->cnt-1) {
            arr->cnt--;
        } else if (idx < arr->cnt) {
            void *ptr_dest = arr->data;
            ptr_dest = ptr_dest + (idx * size);
    
            void *ptr_src = arr->data;
            ptr_src = ptr_src + (arr->cnt-1)*size;
    
            memcpy(ptr_dest, ptr_src, size);
            arr->cnt--;
        }
        if (arr->cnt < arr->capacity/3) {
                _array_resize(size, arr, 0, init_capacity);
        }
    }

}

void _array_insert(size_t size, dynarray_t *arr, size_t idx, void *elem, size_t init_capacity) {
    if (idx == arr->cnt) {
        _array_push(size, arr, elem, init_capacity);
        return;
    }
    if (idx < arr->cnt) {
        if (arr->cnt < arr->capacity) {
            void *ptr_src = arr->data + (idx * size);
            void *ptr_dest = ptr_src + size;
            memcpy(ptr_dest, ptr_src, size * ((arr->cnt)-idx));
            memcpy(ptr_src, elem, size);
            arr->cnt++;
        } else {
            _array_resize(size, arr, 1, init_capacity);
            _array_insert(size, arr, idx, elem, init_capacity);
        }
    }
}

void _array_remove(size_t size, dynarray_t *arr, size_t idx, size_t init_capacity) {
    if (arr->cnt > 0) {
        if (idx == arr->cnt-1) {
            arr->cnt--;
            return;
        }
        if (idx < arr->cnt-1) {
            void *ptr_dest = arr->data + (idx * size);
            void *ptr_src = ptr_dest + size;
            memcpy(ptr_dest, ptr_src, size * ((arr->cnt-1)-idx));
            arr->cnt--;
        }
        if (arr->cnt < arr->capacity/3) {
                _array_resize(size, arr, 0, init_capacity);
        }
    }
}

// void *_array_pop_front(size_t size, dynarray_t *arr, size_t init_capacity) {
//     void *ptr = NULL;
//     if (arr->cnt > 0) {
//         memcpy(ptr, arr->data, size);
//         _array_remove(size, arr, 0,init_capacity);
//     }
//     return ptr;
// }

void _array_free(dynarray_t *arr) {
    free(arr->data);
    free(arr);
}

void _array_concat(size_t size, dynarray_t *dest, dynarray_t *other) {
    if (dest->capacity > dest->cnt + other->cnt) {
        void *ptr_dest = dest->data + size * dest->cnt;
        void *ptr_src = other->data;
        memcpy(ptr_dest, ptr_src, size * (other->cnt));

        dest->cnt = dest->cnt + other->cnt;
        _array_free(other);
    } else {
        dest->data = realloc(dest->data, size*(dest->capacity + other->cnt));
        dest->capacity = dest->capacity + other->cnt;
        _array_concat(size, dest, other);
    }
}