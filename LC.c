#include <stdio.h>
#include "term.h"
#include "parser.h"
#include "delta.h"
#include "alpha.h"
#include "beta.h"

typedef enum : char {
    Beta,
    Delta,
    BetaDelta,
} Startegy;

struct ProgramState { // 128 bytes
    map_t map;
    term_t term;
    term_t acc;

    Startegy strategy;
    bool print;
};

// Perform successive beta and delta reductions until term is normalized
response normalize(term_t* term, term_t* acc, map_t* expressions, bool print) {
    term_t swp;
    
    while (true) {
        if (print)
            lambda_print(term);
        // Perform beta-reduction
        switch (beta_reduce(term, acc)) {
            case Reduced: // If reduction occured, swap terms and repeat
                swp = *term;
                *term = *acc;
                *acc = swp;
                continue;
            case MemoryLow: {
                num_t* new_acc_n = realloc(acc->values, acc->capacity * 2 * sizeof(num_t));
                if (new_acc_n == NULL) {
                    return MemoryUnallocated;
                }
                acc->values = new_acc_n;
                acc->capacity *= 2;
                continue;
            }
            case MemoryUnallocated:
                return MemoryUnallocated;
            case Normal: // If term beta-normalized...
                break;
        }
        // Perform delta-reduction
        switch (delta_reduce(term, acc, expressions)) {
            case Reduced: // If reduction occured, go back to loop start
                swp = *term;
                *term = *acc;
                *acc = swp;
                continue;
            case MemoryLow: {
                num_t* new_acc_n = realloc(acc->values, acc->capacity * 2 * sizeof(num_t));
                if (new_acc_n == NULL) {
                    return MemoryUnallocated;
                }
                acc->values = new_acc_n;
                acc->capacity *= 2;
                continue;
            }
            case MemoryUnallocated:
                return MemoryUnallocated;
            case Normal: // If term is both beta- and delta-normalized...
                break;
        }
        break;
    }
    // Rename variables to minimize space
    alpha_reduce(term, 3);
    return Normal;
}
response add_expression(map_t* map, const term_t* term, num_t id) {
    // Check if id isn't already defined
    size_t map_pos = bst_find_map(map, id);
    if (map_pos != SIZE_MAX)
        return Normal;
    
    // If needed, allocate more space for expression map
    if (map->size == map->capacity) {
        term_t* new_map_n = (term_t*)realloc(map->entries, map->capacity * 2 * sizeof(term_t));
        if (new_map_n == NULL) {
            return MemoryUnallocated;
        }
        map->capacity *= 2;
        map->entries = new_map_n;
    }
    num_t* exp_n = malloc(term->size * sizeof(term_t));
    
    for (size_t i = 0; i < term->size; ++i) {
        exp_n[i] = term->values[i];
    }
    term_t exp = {term->size, term->size, id, exp_n};
    map->entries[(map->size)++] = exp;
    
    insertion_sort_map(map);
    
    return Reduced;
}

void text_parse(const char input[], struct ProgramState* state) {
    if (input[0] == '\n') {
        return;
    }
    if (input[0] == '=') {
        num_t id = exp_parse(input + 1);
        switch(add_expression(&state->map, &state->term, id)) {
            case Normal:
                printf("! Expression already defined\n");
                break;
            case MemoryUnallocated:
                printf("! Memalloc error\n");
            default:
                break;
        }
        return;
    }
    lambda_parse(input, &state->term);
    if (state->term.size == 0) {
        return;
    }
    switch(normalize(&state->term, &state->acc, &state->map, true)) {
        case MemoryUnallocated:
            printf("! Memalloc error\n");
            break;
        case Normal:
            lambda_print(&state->term);
            break;
        default:
            printf("Unexpected instruction\n");
    }
}

// TRUE = \a b.a
// FALSE = \a b.b

// ZERO = \a b.b
// ONE = \a b.a b
// TWO = \a b.a(a b)
// THREE = \a b.a(a(a b))
// FOUR = \a b.a(a(a(a b)))
// INC = \a b c.b(a b c)
// ADD = \a b. a INC b
// MULT = \a b. a (ADD b) ZERO

// PAIR = \a b c.c a b
// FST = \p.p TRUE
// SND = \p.p FALSE

// FACTX = \a. PAIR (MULT (FST a) (SND a)) (INC (SND a))
// FACT = \a.FST (a FACTX (PAIR ONE ONE))

// FACT = \a. (\p.p (\x y.x)) (a (\a1. (\a2 b2 c2.c2 a2 b2) ((\a3 b3. a3 ((\a4 b4. a4 (\a5 b5 c5.b5(a5 b5 c5)) b4) b3) (\a6 b6.b6)) ((\p7.p7 (\a8 b8.a8)) a1) ((\p9.p9 (\a10 b10.b10)) a1)) ((\a11 b11 c11.b11(a11 b11 c11)) ((\p12.p12 (\a13 b13.b13)) a1))) ((\a14 b14 c14.c14 a14 b14) (\a15 b15.a15 b15) (\a16 b16.a16 b16)))
// FACT = \a.a (\b c.c (b (\j k.j) (\d.b (\l m.m) (\e f g.f (e f g)) d) (\h i.i)) (\n o.n (b (\p q.q) n o))) (\r.r (\s t.s t) (\u v.u v)) 

// (\a.a (\d e.e (d (\l m.l) (\f.d (\n o.o) (\g h i.h (g h i)) f) (\j k.k)) (\p q.p (d (\r s.s) p q))) (\t.t (\u v.u v) (\w x.w x)) (\b c.b)) (\a1 b1.a1(a1(a1(a1 b1))))

#define MEM_MIN 16

int main() {
    struct ProgramState state;
    term_t* map_n = malloc(MEM_MIN * sizeof(term_t));
    state.map = (map_t){MEM_MIN, 0, map_n};
    
    num_t* term_n = calloc(MEM_MIN, sizeof(num_t));
    state.term = (term_t){MEM_MIN, 0, SIZE_MAX, term_n};
    
    num_t* acc_n = calloc(MEM_MIN, sizeof(num_t));
    state.acc = (term_t){MEM_MIN, 0, SIZE_MAX, acc_n};
    
    char input[1024];
    
    while (true) {
        printf("> ");
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        text_parse(input, &state);
    }
    //TODO: free expressions from map
    free(map_n);
    free(term_n);
    free(acc_n);
    
    return 0;
}
