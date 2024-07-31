//#include <stdint.h>
//#include <stdlib.h>
#include <stdio.h>
#include "arrays.h"
#include "alpha.h"

int main() {
    
    num_t src[57] = {0,0,1,7,1,8,0,0,7,0,1,5,1,6,0,0,5,1,2,1,3,1,4,0,3,0,0,2,3,4,6,8,1,9,1,10,10,1,11,1,12,0,11,0,11,0,11,12,1,13,1,14,0,13,0,13,14};
    num_t dst[13] = {1,2,1,3,1,4,0,3,0,0,2,3,4};
    
    lambda_print(src);
    lambda_print(dst);
    
    alpha_reduce(dst,13, get_max(src,57));
    
    lambda_print(dst);
    
    return 0;
}
