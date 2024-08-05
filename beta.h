#ifndef _BETA_H
#define _BETA_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "arrays.h"
#include "alpha.h"
#include "types.h"

char beta_reduce(const array_t from, array_t to, size_t* term_size) {
    num_t* from_v = from.values;
    num_t* to_v = to.values;
    
    // Find Applicable Abstraction
    size_t app_pos = 0;
    for (; app_pos < *term_size; ++app_pos) {
        if (from_v[app_pos] == 0)
            if (from_v[app_pos+1] == 1)
                break;
    }
    if (app_pos == *term_size)
        return 'D';
    
    // Parse body and count var occurences
    size_t arg_pos = app_pos+3;
    size_t depth = 1;
    num_t var = from_v[app_pos+2];
    size_t var_count_in_body = 0;
    
    while (depth > 0 && arg_pos < *term_size) {
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
    while (depth > 0 && end_pos < *term_size) {
        if (from_v[end_pos] == 2) depth += 1;
        if (from_v[end_pos] <= 1) depth += 2;
        if (from_v[end_pos] == 1) ++abs_count_in_arg;
        --depth;
        ++end_pos;
    }
    size_t arg_size = end_pos - arg_pos;
    size_t new_size = *term_size + (arg_size - 1) * (var_count_in_body - 1) - 4;

    // If expanded term wont fit in memory return
    if (new_size > to.size)
        return 'M';
    
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
            array_t argument = {arg_size, to.values + alpha_pos};
            char repl = alpha_reduce(argument, *term_size, max_val);
            if (repl == 'M')
                return 'M';
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
    for (size_t pos = end_pos; pos < *term_size; ++pos) {
        to_v[write_pos++] = from_v[pos];
    }
    *term_size = new_size;
    
    return 'S';
}

#endif //_BETA_H
