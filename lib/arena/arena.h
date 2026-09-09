/*  arena.h - arena allocator library
    version 0.1.0 - Iain Dorsch - 2026

    To use this library, do the following in *one* of your .c files:
        #define ARENA_IMPLEMENTATION
        #include "arena.h"
    In any other .c file that needs the library, just include the header:
        #include "arena.h"

    Too avoid collisions with other libraries, the standard API uses a "da_" prefix.
    Short names are also provided. 
    If prefix names are required short names can be disabled:
        #define ARENA_NO_SHORT_NAMES

    Unit tests can be enabled by including the library as described above and 
    defining ARENA_UNIT_TESTS. The function _da_arena_unit_tests() must then be called to run the tests.
    Example:
        #define ARENA_IMPLEMENTATION
        #define ARENA_UNIT_TESTS
        #include "arena.h"

        int main() {
            _da_arena_unit_tests();
            return 0;
        }

    Functionality provided by this library:
        - initialization of contigous memory buffer of a specified size
        - simple allocation, pushing to the end of the buffer
        - simple reallocation, keeping buffer or relying on simple allocation
        - dynamic allocation, use previously freed blocks to allocate to if possible (linear search through blocks)
        - dynamic reallocation, can keep buffer or rely on dynamic allocation.
        - free blocks in the arena
        - reset arena: declare arena as unused = new arena without new allocation
        - uninitialize arena

    Table of contents:
        - Library instructionss
        - Helper macros and struct definitions
        - short name API
        - Internal functions declarations
        - prefix name API
        - Internal functions implementations
*/

#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>

// // // // // // // // // // // // // // // 
// Helper macros
//
// These macros are used to navigate the arena like a list. 
// These operations are required to enable reallocation,
// by saving information about single blocks in a "header" before the pointer to the block.
// This "header" can also be used to traverse the list block by block and aid in defragmentation and freeing of blocks.
//
// LAYOUT EXAMPLE: |HEADER|BLOCK_PTR|...|HEADER|BLOCK_PTR|...
// 
// Get the header for a given block in the arena.
//                             <-----
// Performs this operation ...|HEADER|BLOCK_PTR|...
#define get_header(ptr) ((arena_header_t *) (ptr) - 1)
// Get the next block in the arena based on the previous block ptr
//
// get_next_ptr(ptr) must only be used after it is checked if this pointer lies beyond the current arena->offset
// and only if the next pointer is within the current arena->offset it can be used safely
//                                    ------------------->
// Performs this operation ...|HEADER|BLOCK_PTR|...|HEADER|BLOCK_PTR|...
#define get_next_ptr(ptr) ((ptr) + ((get_header((ptr))->block_size) + (sizeof(arena_header_t))))
// Get first block in the arena
//
// get_first_block(arena) must only be used when arena->offset > 0
// Otherwise the retrieved pointer is unsafe and meaningless.
//                          ----->
// Performs this operation |HEADER|BLOCK_PTR|...
#define get_first_block(arena) ((arena)->base + (sizeof(arena_header_t)))
// // // // // // // // // // // // // // //
typedef struct {
    size_t committed;
    size_t block_size;
} arena_header_t;
 // // // // // // // // // // // // // // //
typedef struct {
    size_t offset;
    size_t capacity;
    char * base;
} arena_t;
// // // // // // // // // // // // // // //
extern void _da_arena_unit_tests(void);
// // // // // // // // // // // // // // // 
// Short names API
#ifndef ARENA_NO_SHORT_NAMES

#define arena_init          da_arena_init
#define arena_alloc         da_arena_alloc
#define arena_realloc       da_arena_realloc
#define arena_free          da_arena_free
#define arena_reset         da_arena_reset
#define arena_uninit        da_arena_uninit
#define arena_dyn_alloc     da_arena_dyn_alloc
#define arena_dyn_realloc   da_arena_dyn_realloc

#endif
// #if !defined(ARENA)
// #error "define ARENA to use arena functionality"
// #endif

// // // // // // // // // // // // // // // 
// Internal functions
extern arena_t * _arena_init(size_t size);
extern void * _arena_alloc(arena_t *arena, size_t size);
extern void * _arena_realloc(arena_t *arena, void *ptr, size_t size);
extern void _arena_free(void *ptr);
extern void _arena_reset(arena_t *arena);
extern void _arena_uninit(arena_t *arena);
// // // // // // // // // // // // // // // 
// No short names API
#define da_arena_init(size) _arena_init(size)
#define da_arena_alloc(arena, size) _arena_alloc(arena, size)
#define da_arena_realloc(arena, ptr, size) _arena_realloc(arena, ptr, size)
#define da_arena_free(ptr) _arena_free(ptr)
#define da_arena_reset(arena) _arena_reset(arena)
#define da_arena_uninit(arena) _arena_uninit(arena)
#define da_arena_dyn_alloc(arena, size) _arena_dyn_alloc(arena,size)
#define da_arena_dyn_realloc(arena, ptr, size) _arena_dyn_realloc(arena,ptr,size)
// // // // // // // // // // // // // // // 
// Internal functions implementations
// #define ARENA_IMPLEMENTATION
#ifdef ARENA_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>

arena_t * _arena_init(size_t size) {
    arena_t * arena = (arena_t *)realloc(NULL, sizeof(arena_t));
    arena->offset = 0;
    arena->capacity = size;
    arena->base = (char *)realloc(NULL, size);
    return arena;
}

void * _arena_alloc(arena_t *arena, size_t size) {
    if (!size) {
        return NULL;
    }
    size_t final_size = size + sizeof(arena_header_t);
    if (arena->capacity < arena->offset + final_size) {
        return NULL;
    }
    char * ret_ptr = arena->base + arena->offset + sizeof(arena_header_t);
    arena->offset += final_size;

    arena_header_t * header = get_header(ret_ptr);
    header->block_size = size;
    header->committed = size;
    
    return (void *)ret_ptr;
}

void * _arena_realloc(arena_t *arena, void *ptr, size_t size) {
    if (ptr != NULL) {
        arena_header_t * header = get_header(ptr);
        if (header->block_size >= size) {
            header->committed = size;
            return ptr;
        }
        if ((header->block_size + sizeof(arena_header_t)) >= arena->offset && size > header->block_size) { // if block ist last block size of block can be increased in-place
            arena->offset += (size - header->block_size);
            header->block_size = size;
            header->committed = size; 
        }
        void * ret_ptr = _arena_alloc(arena, size);
        memmove_s(ret_ptr, size, ptr, header->block_size);
        header->committed = 0;
        return ret_ptr;
    }
    return _arena_alloc(arena, size);
}

void _arena_free(void *ptr) {
    arena_header_t * header = get_header(ptr);
    header->committed = 0;
    (void)ptr;
}

void _arena_reset(arena_t *arena) {
    arena->offset = 0;
}

void _arena_uninit(arena_t *arena) {
    free(arena->base);
    free(arena);
}

// // // // // // // // // // // // // // // 
// Functions that try to allocate in the first space large enough in the arena (take freed space into account)
void * _arena_dyn_alloc(arena_t *arena, size_t size) {
    // arena has content
    size_t offset = arena->offset;
    size_t current_size = 0;
    if (offset > 0) { // if there are blocks in the arena
        char *current_block = get_first_block(arena);
        while (offset > current_size) { // while the current traversale depth has not reached the end of the arena 
            if (get_header(current_block)->committed > 0) { // if the current bllock is free
                current_size += (get_header(current_block)->block_size + sizeof(arena_header_t));
                if ((current_size + sizeof(arena_header_t)) < offset) { // if a next header is still in the current list of blocks
                    current_block = get_next_ptr(current_block);
                } else { // if next header would be outside of already allocated space break while
                    break;
                }
            } else { // free block of sufficient size to hold size was found
                if (get_header(current_block)->block_size >= size) { // if size fits in the block commit size to block and return block ptr
                    get_header(current_block)->committed = size;
                    return current_block;
                }
            }
        }
        // no free block of sufficient size found
        return _arena_alloc(arena, size);
    }
    // arena has no content
    return _arena_alloc(arena, size);
}

void * _arena_dyn_realloc(arena_t *arena, void *ptr, size_t size) {
    // existing block is reassigned/re-sized
    if (ptr != NULL) {
        arena_header_t * header = get_header(ptr);
        if (header->block_size >= size) {
            header->committed = size;
            return ptr;
        }
        if ((header->block_size + sizeof(arena_header_t)) >= arena->offset && size > header->block_size) { // if block ist last block size of block can be increased in-place
            arena->offset += (size - header->block_size);
            header->block_size = size;
            header->committed = size; 
        }
        void * ret_ptr = _arena_dyn_alloc(arena, size);
        memmove_s(ret_ptr, size, ptr, header->block_size);
        header->committed = 0;
        return ret_ptr;
    }
    // no block exists (alloc case)
    return _arena_dyn_alloc(arena, size);
}

#endif

#ifdef ARENA_UNIT_TESTS

#include <stdlib.h>
#include <assert.h>

void _da_arena_unit_tests(void) {
    arena_t *test_arena = _arena_init(40000);

    int * test_one = arena_dyn_realloc(test_arena, NULL, sizeof(int)*20);
    int * test_two = arena_dyn_realloc(test_arena, NULL, sizeof(int)*10);

    test_one[0] = 10;
    test_one[2] = 15;
    test_one[3] = 20;
    test_one[4] = 25;

    test_two[0] = 23;
    
    char * ptr = get_next_ptr(get_first_block(test_arena));
    assert(((int*)ptr)[0] == 23);

    char * first_ptr = get_first_block(test_arena);
    assert(((int*)first_ptr)[0] == 10);

    test_one = arena_dyn_realloc(test_arena,test_one,sizeof(int)*25);

    char * realloc_ptr_one = get_next_ptr(get_next_ptr(get_first_block(test_arena)));
    assert(((int*)realloc_ptr_one)[0] == 10);

    int * test_three = arena_dyn_realloc(test_arena, NULL, sizeof(int)*20);

    test_three[0] = 65;

    char * realloc_ptr_three = get_first_block(test_arena);
    assert(((int*)realloc_ptr_three)[0] == 65);

    test_one = arena_dyn_realloc(test_arena,test_one,sizeof(int)*50);
    test_two = arena_dyn_realloc(test_arena,test_two,sizeof(int)*20);

    realloc_ptr_one = get_next_ptr(get_next_ptr(get_next_ptr(get_first_block(test_arena))));
    char * realloc_ptr_two = get_next_ptr(get_next_ptr(get_first_block(test_arena)));
    assert(((int*)realloc_ptr_one)[0] == 10);
    assert(((int*)realloc_ptr_two)[0] == 23);
}

#endif

#endif