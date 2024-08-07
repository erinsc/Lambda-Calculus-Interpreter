#ifndef _ARRAYS_H
#define _ARRAYS_H

#include <stdint.h>
#include <stdio.h>

typedef enum {
    Reduced,
    Normal,
    MemoryLow,
    MemoryUnallocated,
} response;

typedef uint_fast32_t num_t;

typedef struct {
  size_t size;
  num_t* values;
} array_t;

typedef struct {
    num_t id;
    array_t array;
} entry_t;

typedef struct {
  size_t size;
  entry_t* entries;
} map_t;

num_t get_max(const array_t array) {
    size_t max_val = 0;
    for (size_t pos = 0; pos < array.size; ++pos) {
        max_val = array.values[pos] > max_val ? array.values[pos] : max_val;
    }
    return max_val;
}
size_t bst_find(const array_t array, num_t elem) {
    size_t low = 0;
    size_t high = array.size;
    if (array.values[0] > elem) return SIZE_MAX;
    while (low <= high) {
        size_t mid = low + (high - low) / 2;
        if (array.values[mid] < elem)
            low = mid + 1;
        else if (array.values[mid] > elem)
            high = mid - 1;
        else 
            return mid;
    }
    return SIZE_MAX;
}
size_t bst_find_map(const map_t map, num_t id) {
    size_t low = 0;
    size_t high = map.size;
    entry_t* entries = map.entries;
    if (entries[0].id > id) return SIZE_MAX;
    while (low <= high) {
        size_t mid = low + (high - low) / 2;
        if (entries[mid].id < id)
            low = mid + 1;
        else if (entries[mid].id > id)
            high = mid - 1;
        else 
            return mid;
    }
    return SIZE_MAX;
}
void insertion_sort(array_t array) {
    num_t* values = array.values;
    for (size_t i = 1; i < array.size; ++i) {
        num_t val = values[i];
        size_t j = i;
        while (values[j-1] > val && j > 0) {
            values[j] = values[j-1];
            --j;
        }
        values[j] = val;
    }
}
void insertion_sort_map(map_t map) {
    entry_t* entries = map.entries;
    for (size_t i = 1; i < map.size; ++i) {
        entry_t val = entries[i];
        size_t j = i;
        while (entries[j-1].id > val.id && j > 0) {
            entries[j] = entries[j-1];
            --j;
        }
        entries[j] = val;
    }
}
void arr_copy(const array_t from, array_t to) {
    num_t* to_v = to.values;
    size_t size = from.size > to.size ? to.size : from.size;
    for (size_t i = 0; i < size; ++i) {
        to_v[i] = from.values[i];
    }
}
void arr_print(const array_t term) {
    printf("[%zu", term.values[0]);
    for (size_t i = 1; i < term.size; ++i) {
        printf(", %zu", term.values[i]);
    }
    printf("]\n");
}

#endif //_ARRAYS_H
