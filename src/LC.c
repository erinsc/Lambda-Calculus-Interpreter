#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "term.h"
#include "parser.h"
#include "reduction.h"

#define MEM_MIN 256

typedef struct { // 128 bytes
    map_t map;
    term_t term;
    term_t acc;
    Strategy strategy;
    bool setup;
    bool print;
    bool delta;
} ProgramState;

// Perform successive beta and delta reductions until term is normalized
response normalize(ProgramState* state) {
    term_t swp;
    
    while (true) {
        if (state->print) {
            lambda_print(&state->term);
            printf("\n");
        }
        // Perform beta-reduction
        switch (beta_reduce(&state->term, &state->acc)) {
            case Reduced: // If reduction occured, swap terms and repeat
                swp = state->term;
                state->term = state->acc;
                state->acc = swp;
                continue;
            case MemoryLow: {
                num_t* new_acc_n = realloc(state->acc.values, state->acc.capacity * 2 * sizeof(num_t));
                if (new_acc_n == NULL) {
                    return MemoryUnallocated;
                }
                state->acc.values = new_acc_n;
                state->acc.capacity *= 2;
                continue;
            }
            case MemoryUnallocated:
                return MemoryUnallocated;
            case Normal: // If term beta-normalized...
                break;
        }
        if (!state->delta)
            break;
        
        // Perform delta-reduction
        switch (delta_reduce(&state->term, &state->acc, &state->map)) {
            case Reduced: // If reduction occured, go back to loop start
                swp = state->term;
                state->term = state->acc;
                state->acc = swp;
                continue;
            case MemoryLow: {
                num_t* new_acc_n = realloc(state->acc.values, state->acc.capacity * 2 * sizeof(num_t));
                if (new_acc_n == NULL) {
                    return MemoryUnallocated;
                }
                state->acc.values = new_acc_n;
                state->acc.capacity *= 2;
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
    alpha_reduce(&state->term, 3);
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
void file_parse(const char[], ProgramState*);
void text_parse(const char input[], ProgramState* state) {
    if (input[0] == '\n' || input[0] == '#') {
        return;
    }
    if (input[0] == '-') {
        size_t pos = 1;
        char c = input[pos];
        while ('a' <= c && c <= 'z') {
            if (c == 'h') {
                printf("\"> \" Indicates input prompt.\n");
                printf("Type in \"-\" followed by a character/characters to change settings:\n");
                printf("  \"-d\" Toggles delta reduction, i.e. unpacking of expressions,\n");
                printf("  \"-p\" Toggles printing of intermediate terms,\n");
                printf("  \"-h\" Prints this text,\n");
                printf("  \"-f\" Followed by a filename reads the file and interprets it.\n");
                printf("Type \"=\" to assign last normalized term to given name.\n");
                printf("  Example: \"= ADD\" Assigns last term to name ADD.\n");
                printf("  Name is limited to 6 chars, i.e \"FACTORIAL\" is interpreted as \"FACTOR\".\n");
                printf("Otherwise input is interpreted as lambda term.\n");
                printf("  Character \"\\\" is used instead of the lambda character for input and output.\n");
                printf("  For a more complete explanation of how lambda terms work check out the Github.\n");
                printf("Running the program as \"LC.c <filename>.lc\" Automatically runs lines in order.\n");
                printf("  Lines may be commented out by typing \"#\" as the first character.\n");
            }
            if (c == 'd') 
                state->delta = !state->delta;
            if (c == 'p')
                state->print = !state->print;
            if (c == 'f') {
                char filename[MEM_MIN];
                size_t f_pos = 0;
                c = input[++pos];
                while (c == ' ') c = input[++pos];
                while (c != ' ' && c != '\n' && c != '\t' && c != EOF) {
                    filename[f_pos++] = c;
                    c = input[++pos];
                }
                filename[f_pos] = '\0';
                file_parse(filename, state);
            }
            c = input[++pos];
        }
        return;
    }
    if (input[0] == '=') {
        num_t id = exp_parse(input + 1);
        num_t n[] = {2, id};
        term_t exp = {2,2,id,n};
        switch (add_expression(&state->map, &state->term, id)) {
            case MemoryUnallocated:
                printf("! Memory allocation error\n");
                return;
            case Normal:
                printf("! Expression is already defined: ");
                lambda_print(&exp);
                printf("\n");
                return;
            case Reduced: {
                lambda_print(&exp);
                printf(" = ");
                lambda_print(&state->term);
                printf("\n");
                return;
            }
            default:
                printf("Unhandled case\n");
                return;
        }
    }
    lambda_parse(input, &state->term);
    if (state->term.size == 0) {
        return;
    }
    switch(normalize(state)) {
        case MemoryUnallocated:
            printf("! Memory allocation error\n");
            return;
        case Normal:
            if (!state->setup) {
                lambda_print(&state->term);
                printf("\n");
            }
            return;
        default:
            break;
    }
    printf("Unhandled case\n");
    return;
}
void file_parse(const char filename[], ProgramState* state) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "! Error opening file: %s\n", filename);
        return;
    }
    char input[MEM_MIN];
    state->print = false;
    state->delta = false;
    state->setup = true;
    
    while (fgets(input, sizeof(input), file)) {
        text_parse(input, state);
    }
    fclose(file);
    state->delta = true;
    state->setup = false;
}

int main(int argc, char *argv[]) {
    ProgramState state;
    state.print = false;
    state.delta = false;
    state.setup = true;
    
    term_t* map_n = malloc(MEM_MIN * sizeof(term_t));
    state.map = (map_t){MEM_MIN, 0, map_n};
    
    num_t* term_n = calloc(MEM_MIN, sizeof(num_t));
    state.term = (term_t){MEM_MIN, 0, SIZE_MAX, term_n};
    
    num_t* acc_n = calloc(MEM_MIN, sizeof(num_t));
    state.acc = (term_t){MEM_MIN, 0, SIZE_MAX, acc_n};
    
    char input[MEM_MIN];
    
    if (argc >= 2) {
        for (int i = 1; i < argc; ++i) {
            file_parse(argv[i], &state);
        }
        printf("\n");
    }

    printf("Lambda Calculus Interpreter 1.0 - erinsc 2024\n");
    printf("Type -h for help\n");
    
    while (true) {
        printf("> ");
        
        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }
        text_parse(input, &state);
        printf("\n");
    }
    printf("\n");
    
    return EXIT_SUCCESS;
}
