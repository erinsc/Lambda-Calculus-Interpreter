#include <stdio.h>
#include "arrays.h"
#include "parser.h"
#include "alpha.h"
#include "beta.h"

int main() {
    size_t mem_size = 1024; // Max term size
    //size_t exp_size = 16; 
    char input[] = "(\\a b.a) (\\x.x) u";
    
    num_t* from_n = malloc(mem_size * sizeof(num_t));
    num_t* to_n = malloc(mem_size * sizeof(num_t));
    
    array_t from = {mem_size, from_n};
    array_t to = {mem_size, to_n};
    
    num_t* swp;
    
    //entry_t* expressions = malloc(exp_size * sizeof(struct entry));

    size_t term_size = lambda_parse(input, from);
    
    char ret = 'S';
    while (ret == 'S') {
        lambda_print(from);
        ret = beta_reduce(from, to, &term_size);
        swp = from.values;
        from.values = to.values;
        to.values = swp;
    }
    alpha_reduce(to, term_size, 3);
    lambda_print(to);
    arr_print(to);
    return 0;
}
    
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
