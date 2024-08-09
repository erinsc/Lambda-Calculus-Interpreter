#ifndef _TERM_H
#define _TERM_H

#include <stdint.h>
#include <stdio.h>

typedef enum : char {
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

num_t get_max(const term_t* term) {
    size_t max_val = 2;
    for (size_t pos = 0; pos < term->size; ++pos) {
        if (term->values[pos] == 2) {
            ++pos;
            continue;
        }
        max_val = term->values[pos] > max_val ? term->values[pos] : max_val;
    }
    return max_val;
}
size_t bst_find(const term_t* term, num_t elem) {
    size_t low = 0;
    size_t high = term->size;
    if (term->size == 0) return SIZE_MAX;
    if (term->values[0] > elem) return SIZE_MAX;
    while (low <= high) {
        size_t mid = low + (high - low) / 2;
        if (term->values[mid] < elem)
            low = mid + 1;
        else if (term->values[mid] > elem)
            high = mid - 1;
        else 
            return mid;
    }
    return SIZE_MAX;
}
size_t bst_find_map(const map_t* map, num_t id) {
    size_t low = 0;
    size_t high = map->size;
    if (map->size == 0) return SIZE_MAX;
    if (map->entries[0].id > id) return SIZE_MAX;
    while (low <= high) {
        size_t mid = low + (high - low) / 2;
        if (map->entries[mid].id < id)
            low = mid + 1;
        else if (map->entries[mid].id > id)
            high = mid - 1;
        else 
            return mid;
    }
    return SIZE_MAX;
}
void insertion_sort(term_t* term) {
    num_t* values = term->values;
    for (size_t i = 1; i < term->size; ++i) {
        num_t val = values[i];
        size_t j = i;
        while (values[j-1] > val && j > 0) {
            values[j] = values[j-1];
            --j;
        }
        values[j] = val;
    }
}
void insertion_sort_map(map_t* map) {
    for (size_t i = 1; i < map->size; ++i) {
        term_t val = map->entries[i];
        size_t j = i;
        while (j > 0 && map->entries[j-1].id > val.id) {
            map->entries[j] = map->entries[j-1];
            --j;
        }
        map->entries[j] = val;
    }
}
/*void arr_copy(const term_t* from, array_t to) {
    num_t* to_v = to.values;
    size_t size = from.size > to.size ? to.size : from.size;
    for (size_t i = 0; i < size; ++i) {
        to_v[i] = from.values[i];
    }
}*/
void arr_print(const term_t* term) {
    printf("[%zu", term->values[0]);
    for (size_t i = 1; i < term->size; ++i) {
        printf(", %zu", term->values[i]);
    }
    printf("]\n");
}

#endif //_TERM_H
