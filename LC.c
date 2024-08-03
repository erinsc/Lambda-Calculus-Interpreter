#include <stdio.h>
#include "arrays.h"
#include "parser.h"
#include "alpha.h"
#include "beta.h"

int main() {
    size_t mem_size = 1000; // Max term size
    char input[] = "(\\a b.a INC b) (\\z w. z(z(z(z w))))";
    
    num_t* from = malloc(mem_size * sizeof(num_t));
    num_t* to = malloc(mem_size * sizeof(num_t));
    num_t* swp;
    
    size_t term_size = lambda_parse(input, from);
    
    char ret = 'S';
    while (ret == 'S') {
        lambda_print(from);
        ret = beta_reduce(from, &term_size, to, mem_size);
        swp = from;
        from = to;
        to = swp;
    }
    alpha_reduce(to, term_size, 3);
    lambda_print(to);
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
