#include <stdio.h>
#include "arrays.h"
#include "parser.h"
#include "alpha.h"

int main() {
    
    //"(\\7 8.7 ((\\5 6.5 (\\2 3 4.3 (2 3 4)) 6) 8) (\\9 10.10)) (\\11 12.11 (11 (11 12))) (\\13 14.13 (13 14))";

    //"(\7 8.7 ((\5 6.5 (\2 3 4.3 (2 3 4)) 6) 8) (\9 10.10)) (\11 12.11 (11 (11 12))) (\13 14.13 (13 14))";

    char input[1024];
    num_t output[100] = {0};
    
    while (1) {
        if (scanf(" %1023[^\n]", input) == EOF) break;
        size_t size = lambda_parse(input, output);
        //printf("%zu\n", size);
        if (size == 0)
            continue;
        
        lambda_print(output);
        
    }
    
    return 0;
}
