#ifndef _REDUCTION_H
#define _REDUCTION_H

#include "term.h"

typedef enum {
    Beta,
    Delta,
    BetaDelta,
} Strategy;

response alpha_reduce(term_t* term, size_t max_val);
response beta_reduce(const term_t* from, term_t* to);
response delta_reduce(const term_t* from, term_t* to, const map_t* map);

#endif //_REDUCTION_H
