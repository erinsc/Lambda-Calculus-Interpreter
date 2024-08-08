#ifndef _DELTA_H
#define _DELTA_H

#include <stdint.h>
#include <stdlib.h>
#include "arrays.h"
#include "alpha.h"

response delta_reduce(const array_t from, array_t to, const map_t expressions, size_t* term_size) {
    num_t* from_v = from.values;
    num_t* to_v = to.values;
    
    // Find Applicable Expression
    size_t exp_pos = 0;
    for (; exp_pos < *term_size; ++exp_pos) {
        if (from_v[exp_pos] == 2)
            break;
    }
    if (exp_pos == *term_size)
        return Normal;
    
    size_t map_pos = bst_find_map(expressions, from_v[exp_pos+1]);
    if (map_pos == SIZE_MAX)
        return Normal;
    
    array_t exp = expressions.entries[map_pos].array;
    
    // If expanded term wont fit in memory return
    size_t new_size = *term_size + exp.size - 2;
    if (new_size > to.size)
        return MemoryLow;
    
    // Copy prefix of array
    size_t write_pos = 0;
    for (size_t pos = 0; pos < exp_pos; ++pos) {
        to_v[write_pos++] = from_v[pos];
    }
    // Copy expression
    for (size_t pos = 0; pos < exp.size; ++pos) {
        to_v[write_pos++] = exp.values[pos];
    }
    // Rename variables to avoid conflict
    num_t max_val = 2;
    for (size_t pos = 0; pos < *term_size; ++pos) {
        if (from.values[pos] == 2) {
            ++pos;
            continue;
        }
        max_val = from.values[pos] > max_val ? from.values[pos] : max_val;
    }
    max_val += 1;
    
    array_t argument = {exp.size, to.values + exp_pos};
    
    if (alpha_reduce(argument, exp.size, max_val) == MemoryUnallocated)
        return MemoryUnallocated;
    
    //arr_print(argument);
    
    // Finish copying rest of term
    for (size_t pos = exp_pos+2; pos < *term_size; ++pos) {
        to_v[write_pos++] = from_v[pos];
    }
    *term_size = new_size;
    
    return Reduced;
}
#endif //_DELTA_H
