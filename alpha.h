#ifndef _ALPHA_H
#define _ALPHA_H

#include <stdint.h>
#include <stdlib.h>
#include "arrays.h"

typedef uint_fast32_t num_t;

char alpha_reduce(num_t term[], size_t size, size_t max_val) {
    // Count abs occurences
    size_t abs_count_in_side = 0;
    for (size_t pos = 0; pos < size; ++pos) {
        if (term[pos] == 1)
            ++abs_count_in_side;
    }
    // Allocate replacing chart
    num_t* repl_chart = (num_t*)malloc(abs_count_in_side * sizeof(num_t));
    if (repl_chart == NULL)
        return 'M';
        
    // Generate chart
    size_t repl_pos = 0;
    for (size_t pos = 0; pos < size; ++pos) {
        if (term[pos] == 1)
            repl_chart[repl_pos++] = term[++pos];
    }
    insertion_sort(repl_chart, abs_count_in_side);
    
    // Perform alpha reduction
    for (size_t pos = 0; pos < size; ++pos) {
        if (term[pos] == 2) {
            ++pos;
            continue;
        }
        size_t found = bst_find(repl_chart, abs_count_in_side, term[pos]);
        if (found != SIZE_MAX)
            term[pos] = max_val + found;
    }
    
    return 'S';
}

#endif //_ALPHA_H
