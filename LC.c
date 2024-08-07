#include <stdio.h>
#include "arrays.h"
#include "parser.h"
#include "delta.h"
#include "alpha.h"
#include "beta.h"

// Perform successive beta and delta reductions until term is normalized
response reduce(array_t* term, map_t expressions, size_t* term_size, bool print) {
    // Allocate accumulator buffer for inbetween results
    num_t* acc_n = malloc(term->size * sizeof(num_t));
    if (acc_n == NULL)
        return MemoryUnallocated;
    array_t acc_a = {term->size, acc_n};
    array_t* acc = &acc_a;
    array_t swp;
    
    while (true) {
        if (print)
            lambda_print(*term);
        
        switch (beta_reduce(*term, *acc, term_size)) {
            case Reduced:
                swp = *term;
                *term = *acc;
                *acc = swp;
                continue;
            case MemoryLow: {
                num_t* new_acc_n = realloc(acc->values, acc->size * 2 * sizeof(num_t));
                if (new_acc_n == NULL) {
                    free(acc->values);
                    return MemoryUnallocated;
                }
                acc->values = new_acc_n;
                acc->size *= 2;
                continue;
            }
            case MemoryUnallocated:
                free(acc->values);
                return MemoryUnallocated;
            case Normal:
                break;
        }
        switch (delta_reduce(*term, *acc, expressions, term_size)) {
            case Reduced:
                swp = *term;
                *term = *acc;
                *acc = swp;
                continue;
            case MemoryLow: {
                num_t* new_acc_n = realloc(acc->values, acc->size * 2 * sizeof(num_t));
                if (new_acc_n == NULL) {
                    free(acc->values);
                    return MemoryUnallocated;
                }
                acc->values = new_acc_n;
                acc->size *= 2;
                continue;
            }
            case MemoryUnallocated:
                free(acc->values);
                return MemoryUnallocated;
            case Normal:
                break;
        }
        break;
    }
    num_t* new_values = realloc(term->values, *term_size * sizeof(num_t));
    if (new_values != NULL) 
        term->values = new_values;
    term->size = *term_size;
    alpha_reduce(*term, *term_size, 3);
    return Normal;
}

int main() {
    size_t map_size = 16; 
    entry_t* exps_arr = malloc(map_size * sizeof(entry_t));
    const num_t inc1[] = {1, 4, 1, 3, 1, 5, 0, 3, 0, 0, 4, 3, 5};
    num_t* inc2 = malloc(13 * sizeof(entry_t));
    
    for (size_t i = 0; i < 13; ++i) {
        inc2[i] = inc1[i];
    }
    entry_t inc3 = {2404, {13, inc2}};
    exps_arr[0] = inc3;
    map_t expressions = {1, exps_arr};
    
    char input[1024];
    num_t* term_n = malloc(1000 * sizeof(num_t));
    array_t term = {1000, term_n};
    
    printf("> ");
    if (scanf(" %1023[^\n]", input) == EOF) {
        return 1;
    }
    size_t term_size = lambda_parse(input, term);
    if (term_size == 0) {
        return 1;
    }
    
    switch(reduce(&term, expressions, &term_size, true)) {
        case MemoryUnallocated:
            return 2;
        default:
            lambda_print(term);
            arr_print(term);
    }
}
    /*size_t map_size = 16; 
    entry_t* exps_arr = malloc(map_size * sizeof(entry_t));
    const num_t inc1[] = {1, 4, 1, 3, 1, 5, 0, 3, 0, 0, 4, 3, 5};
    num_t* inc2 = malloc(13 * sizeof(entry_t));
    
    for (size_t i = 0; i < 13; ++i) {
        inc2[i] = inc1[i];
    }
    entry_t inc3 = {2404, {13, inc2}};
    exps_arr[0] = inc3;
    map_t expressions = {1, exps_arr};
    
    
    
    size_t mem_size = 40; // Max term size
    
    //char input[] = "(\n f x.f(n f x))";
    char input[] = "(\a b.a INC b) (\f x.f(f(f x)))";
    
    num_t* term_n = malloc(mem_size * sizeof(num_t));
    array_t term = {mem_size, term_n};
    
    size_t term_size = lambda_parse(input, term);
    
    reduce(&term, expressions, &term_size, true);
    
    lambda_print(term);
    arr_print(term);
    
    return 0;
    */
    
    /*
    while (1) {
        printf("> ");
        if (scanf(" %1023[^\n]", input) == EOF) break;
        size_t term_size = lambda_parse(input, from);
        if (term_size == 0)
            continue;

        char ret = 'S';
        while (ret == 'S') {
            ret = beta_reduce(from, &term_size, to, mem_size);
            
            swp = from;
            from = to;
            to = swp;
            
            lambda_print(to);
        }
        
        
        
    }*/
