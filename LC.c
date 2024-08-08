#include <stdio.h>
#include "arrays.h"
#include "parser.h"
#include "delta.h"
#include "alpha.h"
#include "beta.h"

typedef enum {
    Beta,
    Delta,
    BetaDelta,
} startegy;

struct ProgramState {
    map_t map;
    size_t map_size;
    array_t term;
    size_t term_size;
};

// Perform successive beta and delta reductions until term is normalized
response normalize(array_t* term, map_t expressions, size_t* term_size, bool print) {
    // Allocate accumulator buffer for inbetween results
    num_t* acc_n = malloc(term->size * sizeof(num_t));
    if (acc_n == NULL)
        return MemoryUnallocated;
    array_t acc = {term->size, acc_n};
    array_t swp;
    
    while (true) {
        if (print)
            lambda_print(*term);
        // Perform beta-reduction
        switch (beta_reduce(*term, acc, term_size)) {
            case Reduced: // If reduction occured, swap terms and repeat
                swp = *term;
                *term = acc;
                acc = swp;
                continue;
            case MemoryLow: {
                num_t* new_acc_n = realloc(acc.values, acc.size * 2 * sizeof(num_t));
                if (new_acc_n == NULL) {
                    free(acc.values);
                    return MemoryUnallocated;
                }
                acc.values = new_acc_n;
                acc.size *= 2;
                continue;
            }
            case MemoryUnallocated:
                free(acc.values);
                return MemoryUnallocated;
            case Normal: // If term beta-normalized...
                break;
        }
        // Perform delta-reduction
        switch (delta_reduce(*term, acc, expressions, term_size)) {
            case Reduced: // If reduction occured, go back to loop start
                swp = *term;
                *term = acc;
                acc = swp;
                continue;
            case MemoryLow: {
                num_t* new_acc_n = realloc(acc.values, acc.size * 2 * sizeof(num_t));
                if (new_acc_n == NULL) {
                    free(acc.values);
                    return MemoryUnallocated;
                }
                acc.values = new_acc_n;
                acc.size *= 2;
                continue;
            }
            case MemoryUnallocated:
                free(acc.values);
                return MemoryUnallocated;
            case Normal: // If term is both beta- and delta-normalized...
                break;
        }
        break;
    }
    // Rename variables to minimize space
    alpha_reduce(*term, *term_size, 3);
    return Normal;
}
response add_expression(map_t* map, size_t* map_size, const array_t term, size_t term_size, num_t id) {
    // Check if id isn't already defined
    size_t map_pos = bst_find_map(*map, id);
    if (map_pos != SIZE_MAX)
        return Normal;
    
    // If needed, allocate more space for expression map
    if (*map_size >= map->size) {
        entry_t* new_map_n = realloc(map->entries, *map_size * 2 * sizeof(entry_t));
        if (new_map_n == NULL) {
            return MemoryUnallocated;
        }
        map->size *= 2;
        map->entries = new_map_n;
    }
    num_t* exp_n = malloc(term_size * sizeof(entry_t));
    
    for (size_t i = 0; i < term_size; ++i) {
        exp_n[i] = term.values[i];
    }
    array_t exp = {term_size, exp_n};
    entry_t entry = {id, exp};
    map->entries[(*map_size)++] = entry;
    
    insertion_sort_map(*map);
    
    return Reduced;
}

void text_parse(const char input[], struct ProgramState* state) {
    if (input[0] == '\n') {
        return;
    }
    if (input[0] == '=') {
        num_t id = exp_parse(input + 1);
        switch(add_expression(&state->map, &state->map_size, state->term, state->term_size, id)) {
            case Normal:
                printf("! Expression already defined\n");
                break;
            case MemoryUnallocated:
                printf("! Memalloc error\n");
            default:
        }
        return;
    }
    state->term_size = lambda_parse(input, state->term);
    if (state->term_size == 0) {
        return;
    }
    
    switch(normalize(&state->term, state->map, &state->term_size, true)) {
        case MemoryUnallocated:
            printf("! Memalloc error\n");
            break;
        case Normal:
            lambda_print(state->term);
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

int main() {
    struct ProgramState state = {.map_size=0, .term_size=0};
    entry_t* map_n = malloc(16 * sizeof(entry_t));
    state.map = (map_t){16, map_n};
    
    num_t* term_n = malloc(16 * sizeof(num_t));
    state.term = (array_t){16, term_n};
    
    char input[1024];
    
    while (true) {
        printf("> ");
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        text_parse(input, &state);
    }
}
