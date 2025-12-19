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

#define arr_reserve(Type, size) (dynarray_t *)_array_init(sizeof(Type), size)

__attribute__((warn_unused_result,malloc))
static inline dynarray_t *_array_empty() {
    dynarray_t *result = (dynarray_t *)malloc(sizeof(dynarray_t));
    
    memset(result, 0, sizeof(dynarray_t));
    return result;
}
#define arr(Type) (dynarray_t *)_array_init(sizeof(Type), MIN_CAPACITY)
__attribute__((warn_unused_result,malloc))
static inline dynarray_t *_array_init(size_t size, size_t init_capacity) {
    dynarray_t *ptr = _array_empty();
    ptr->data = (char *)malloc(size*init_capacity);
    // memset(ptr->data, 0, size*init_capacity);
    ptr->capacity = init_capacity;
    return ptr;
}

#define arr_set(Type,array,index,elem) _array_set(sizeof(Type),array,index,elem)
__attribute((nonnull(2)))
static inline void _array_set(size_t size, dynarray_t *arr, size_t idx, void *elem) {
    if (idx < arr->cnt) {
        char *ptr = arr->data;
        ptr = ptr + (idx * size);
        memcpy(ptr, elem, size);
    }
}

#define arr_with(Type, count, elem) (dynarray_t *)_array_init_with(sizeof(Type), count, elem)
__attribute__((nonnull(3),malloc,warn_unused_result))
static inline dynarray_t * _array_init_with(size_t size, size_t count, void * elem) {
    dynarray_t *ptr = _array_empty();
    ptr->data = (char *)malloc(size*count);
    ptr->capacity = count;
    ptr->cnt = count;
    for (size_t i = 0; i < count; i++) {
        _array_set(size,ptr,i,elem);
    }
    return ptr;
}

#define arr_foreach(Type, item, array) for (Type *item = (Type *)array->data; item < (Type *)(array->data + array->cnt * sizeof(Type)); item++)

#define arr_filter(Type, item, array, condition) \
    for (Type *item = (Type *)array->data; \
         item < (Type *)(array->data + array->cnt * sizeof(Type)); \
         item++) \
        if (condition)

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

#define arr_push(Type,array,elem) _array_push(sizeof(Type),array,elem, MIN_CAPACITY)
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

#define arr_free(array) _array_free(array)
__attribute__((nonnull(1)))
static inline void _array_free(dynarray_t *arr) {
    free(arr->data);
    free(arr);
}

#define arr_swap_remove(Type,array,index) _array_swap_remove(sizeof(Type),array,index, MIN_CAPACITY)
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

#define arr_pop(Type,array) (Type *)_array_pop(sizeof(Type), array, MIN_CAPACITY)
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

#define arr_get(Type,array,index) (Type *)_array_get(sizeof(Type),array,index)
__attribute__((hot,warn_unused_result,hot,nonnull(2)))
static inline void *_array_get(size_t size, dynarray_t *arr, size_t idx) {
    char *ptr = NULL;
    if (idx < arr->cnt) {
        ptr = arr->data + (idx * size);
    }
    return ptr;
}

#define arr_peek(Type,array) arr_get(Type,array,array->cnt-1)

#define arr_insert(Type,array,index,elem) _array_insert(sizeof(Type),array,index,elem, MIN_CAPACITY)
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

#define arr_remove(Type,array,index) _array_remove(sizeof(Type),array,index, MIN_CAPACITY)
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

#define arr_push_front(Type,array,elem) arr_insert(Type,array,0,elem)

#define arr_concat(Type,array_one,array_two) _array_concat(sizeof(Type),array_one,array_two)
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