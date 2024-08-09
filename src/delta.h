#ifndef _DELTA_H
#define _DELTA_H

#include <stdint.h>
#include <stdlib.h>
#include "term.h"
#include "alpha.h"
#include "parser.h"

response delta_reduce(const term_t* from, term_t* to, const map_t* map) {
    num_t* from_v = from->values;
    num_t* to_v = to->values;
    
    // Find Applicable Expression
    size_t exp_pos = 0;
    for (; exp_pos < from->size; ++exp_pos) {
        if (from_v[exp_pos] == 2)
            break;
    }
    if (exp_pos == from->size)
        return Normal;
    
    size_t map_pos = bst_find_map(map, from_v[exp_pos+1]);
    if (map_pos == SIZE_MAX)
        return Normal;
    
    term_t exp = map->entries[map_pos];
    
    // If expanded term wont fit in memory return
    size_t new_size = from->size + exp.size - 2;
    if (new_size > to->capacity)
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
    num_t max_val = get_max(from) + 1;
    
    term_t argument = {exp.size, exp.size, SIZE_MAX, to->values + exp_pos};
    
    if (alpha_reduce(&argument, max_val) == MemoryUnallocated)
        return MemoryUnallocated;
    
    //arr_print(argument);
    
    // Finish copying rest of term
    for (size_t pos = exp_pos+2; pos < from->size; ++pos) {
        to_v[write_pos++] = from_v[pos];
    }
    to->size = new_size;
    
    return Reduced;
}
#endif //_DELTA_H
