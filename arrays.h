#ifndef _ARRAYS_H
#define _ARRAYS_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

typedef uint_fast32_t num_t;

num_t get_max(const num_t arr[], size_t size) {
    size_t max_val = 0;
    for (size_t pos = 0; pos < size; ++pos) {
        max_val = arr[pos] > max_val ? arr[pos] : max_val;
    }
    return max_val;
}
size_t bst_find(const num_t arr[], size_t size, num_t elem) {
    size_t low = 0;
    size_t high = size;
    if (arr[0] > elem) return SIZE_MAX;
    while (low <= high) {
        size_t mid = low + (high - low) / 2;
        if (arr[mid] < elem)
            low = mid + 1;
        else if (arr[mid] > elem)
            high = mid - 1;
        else 
            return mid;
    }
    return SIZE_MAX;
}
void arr_copy(const num_t from[], num_t to[], size_t size) {
    for (size_t i = 0; i < size; ++i) {
        to[i] = from[i];
    }
}
void insertion_sort(num_t input[], size_t size) {
    for (size_t i = 1; i < size; ++i) {
        num_t val = input[i];
        size_t j = i;
        while (input[j-1] > val && j > 0) {
            input[j] = input[j-1];
            --j;
        }
        input[j] = val;
    }
}
void arr_print(const num_t term[], size_t size) {
    printf("[%zu", term[0]);
    for (size_t i = 1; i < size; ++i) {
        printf(", %zu", term[i]);
    }
    printf("]\n");
}

#endif //_ARRAYS_H
