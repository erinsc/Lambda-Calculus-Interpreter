#ifndef _ALPHA_H
#define _ALPHA_H

#include <stdint.h>
#include <stdlib.h>
#include "term.h"

response alpha_reduce(term_t* term, size_t max_val) {
    // Count abs occurences
    size_t abs_count_in_side = 0;
    for (size_t pos = 0; pos < term->size; ++pos) {
        if (term->values[pos] == 1)
            ++abs_count_in_side;
    }
    // Allocate replacing chart
    num_t* repl_values = (num_t*)malloc(abs_count_in_side * sizeof(num_t));
    if (repl_values == NULL)
        return MemoryUnallocated;
    term_t repl_chart = {abs_count_in_side, abs_count_in_side, SIZE_MAX, repl_values};
    
    // Generate chart
    size_t repl_pos = 0;
    for (size_t pos = 0; pos < term->size; ++pos) {
        if (term->values[pos] == 1)
            repl_chart.values[repl_pos++] = term->values[++pos];
    }
    insertion_sort(&repl_chart);

    // Perform alpha reduction
    for (size_t pos = 0; pos < term->size; ++pos) {
        if (term->values[pos] == 2) {
            ++pos;
            continue;
        }
        size_t found = bst_find(&repl_chart, term->values[pos]);
        if (found != SIZE_MAX)
            term->values[pos] = max_val + found;
    }
    free(repl_values);
    return Reduced;
}

#endif //_ALPHA_H
