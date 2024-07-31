#include <stdio.h>
#include "arrays.h"
#include "parser.h"
#include "alpha.h"
#include "beta.h"

int main() {
    
    // Multiplication                                        Three (first arg)        Four (second arg)
    //"(\7 8.7 ((\5 6.5 (\2 3 4.3 (2 3 4)) 6) 8) (\9 10.10)) (\11 12.11 (11 (11 12))) (\13 14.13 (13 14))";
    size_t mem_size = 1000; // Max term size
    char input[1024];
    
    num_t* from = malloc(mem_size * sizeof(num_t));
    num_t* to = malloc(mem_size * sizeof(num_t));
    num_t* swp = from;
    
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
    }
    
    return 0;
}
