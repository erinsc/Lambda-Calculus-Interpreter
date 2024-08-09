#ifndef _TERM_H
#define _TERM_H

#include <stdint.h>
#include <stdio.h>

typedef enum {
    Reduced,
    Normal,
    MemoryLow,
    MemoryUnallocated
} response;

typedef uint_fast64_t num_t; // 8 bytes

typedef struct { // 32 bytes
  size_t capacity;
  size_t size;
  num_t id;
  num_t* values;
} term_t;

typedef struct { // 24+8 bytes
  size_t capacity;
  size_t size;
  term_t* entries;
} map_t;

num_t get_max(const term_t* term);
size_t bst_find(const term_t* term, num_t elem);
size_t bst_find_map(const map_t* map, num_t id);
void insertion_sort(term_t* term);
void insertion_sort_map(map_t* map);
void arr_print(const term_t* term);
#endif //_TERM_H
