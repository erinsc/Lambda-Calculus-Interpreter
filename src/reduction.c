#include <stdbool.h>

#include "term.h"
#include "stdlib.h"

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
response beta_reduce(const term_t* from, term_t* to) {
    num_t* from_v = from->values;
    num_t* to_v = to->values;
    
    // Find Applicable Abstraction
    size_t app_pos = 0;
    for (; app_pos < from->size; ++app_pos) {
        if (from_v[app_pos] == 0)
            if (from_v[app_pos+1] == 1)
                break;
    }
    if (app_pos == from->size)
        return Normal;
    
    // Parse body and count var occurences
    size_t arg_pos = app_pos+3;
    size_t depth = 1;
    num_t var = from_v[app_pos+2];
    size_t var_count_in_body = 0;
    
    while (depth > 0 && arg_pos < from->size) {
        if (from_v[arg_pos] == 2) depth += 1;
        if (from_v[arg_pos] <= 1) depth += 2;
        if (from_v[arg_pos] == var) ++var_count_in_body;
        --depth;
        ++arg_pos;
    }
    // Parse arg and count abs occurences
    size_t end_pos = arg_pos;
    size_t abs_count_in_arg = 0;
    depth = 1;
    while (depth > 0 && end_pos < from->size) {
        if (from_v[end_pos] == 2) depth += 1;
        if (from_v[end_pos] <= 1) depth += 2;
        if (from_v[end_pos] == 1) ++abs_count_in_arg;
        --depth;
        ++end_pos;
    }
    size_t arg_size = end_pos - arg_pos;
    size_t new_size = from->size + (arg_size - 1) * (var_count_in_body - 1) - 4;

    // If expanded term wont fit in memory return
    if (new_size > to->capacity)
        return MemoryLow;
    
    // Start Beta reduction
    
    // Copy prefix of array
    size_t write_pos = 0;
    for (size_t pos = 0; pos < app_pos; ++pos) {
        to_v[write_pos++] = from_v[pos];
    }
    // Preform simple beta reduction
    bool first = true;
    size_t alpha_pos;
    num_t max_val;
    size_t read_pos = app_pos + 3;
    for (; read_pos < arg_pos; ++read_pos) {
        if (from_v[read_pos] == 2) {
            to_v[write_pos++] = from_v[read_pos++];
            to_v[write_pos++] = from_v[read_pos];
            continue;
        }
        if (from_v[read_pos] != var) {
            to_v[write_pos++] = from_v[read_pos];
            continue;
        }
        for (size_t pos = arg_pos; pos < end_pos; ++pos) {

            to_v[write_pos++] = from_v[pos];
        }
        // If second variable found, rename bound variables
        if (!first) {
            max_val = get_max(from) + 1;
            alpha_pos = write_pos - arg_size;
            // rename using alpha reduction. if fails for memory, fail
            term_t argument = {arg_size, arg_size, SIZE_MAX, to->values + alpha_pos};
            if (alpha_reduce(&argument, max_val) == MemoryUnallocated)
                return MemoryUnallocated;
            // Check if there are enough free variable names
            //if (max_val - 1 + (var_count_in_body-1) * abs_count_in_arg < max_val)
            //    return 'V';
            ++read_pos;
            break;
        }
        first = false;
    }
    if (var_count_in_body > 1) {
        // Finish beta reduction
        for (; read_pos < arg_pos; ++read_pos) {
            if (from_v[read_pos] == 2) {
                to_v[write_pos++] = from_v[read_pos++];
                to_v[write_pos++] = from_v[read_pos];
                continue;
            }
            if (from_v[read_pos] != var) {
                to_v[write_pos++] = from_v[read_pos];
                continue;
            }
            for (size_t pos = alpha_pos; pos < alpha_pos + arg_size; ++pos) {
                num_t offset = to_v[pos] >= max_val ? abs_count_in_arg : 0;
                to_v[write_pos++] = to_v[pos] + offset;
            }
            alpha_pos = write_pos - arg_size;
        }
    }
    // Finish copying rest of term
    for (size_t pos = end_pos; pos < from->size; ++pos) {
        to_v[write_pos++] = from_v[pos];
    }
    to->size = new_size;
    
    return Reduced;
}
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
