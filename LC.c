#include <stdio.h>
#include "arrays.h"
#include "parser.h"
#include "alpha.h"

int main() {
    
    char input[] = "(\\7 8.7 ((\\5 6.5 (\\2 3 4.3 (2 3 4)) 6) 8) (\\9 10.10)) (\\11 12.11 (11 (11 12))) (\\13 14.13 (13 14))";
    
    num_t output[100] = {0};
    
    printf("%s\n", input);

    lambda_parse(input, output);
    
    lambda_print(output);
    
    return 0;
}
